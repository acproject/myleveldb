//
// Created by qinhangyu on 23-8-23.
//

#ifndef MYLEVEDB_CACHE_H
#define MYLEVEDB_CACHE_H
#include <cstdint>

#include "export.h"
#include "slice.h"

namespace myleveldb {
 class LEVELDB_EXPORT Cache;

 // Create a new cache with a fixed size capacity.  This implementation
// of Cache uses a least-recently-used eviction policy.
 LEVELDB_EXPORT Cache* NewLRUCache(size_t capacity);

 class LEVELDB_EXPORT Cache {
 public:
     Cache() = default;

     Cache(const Cache&) = delete;

     Cache& operator=(const Cache&) = delete;

     virtual ~Cache();

     struct Handle {};

     virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                            void (*deleter)(const Slice& key, void* value)) = 0;

     virtual Handle* Lookup(const Slice& key) = 0;

     // Release a mapping returned by a previous Lookup().
     // REQUIRES: handle must not have been released yet.
     // REQUIRES: handle must have been returned by a method on *this.
     virtual void Release(Handle* handle) = 0;

     // Return the value encapsulated in a handle returned by a
     // successful Lookup().
     // REQUIRES: handle must not have been released yet.
     // REQUIRES: handle must have been returned by a method on *this.
     virtual void* Value(Handle* handle) = 0;

     // If the cache contains entry for key, erase it.  Note that the
     // underlying entry will be kept around until all existing handles
     // to it have been released.
     virtual void Erase(const Slice& key) = 0;

     // Return a new numeric id.  May be used by multiple clients who are
     // sharing the same cache to partition the key space.  Typically the
     // client will allocate a new id at startup and prepend the id to
     // its cache keys.
     virtual uint64_t NewId() = 0;

     // Remove all cache entries that are not actively in use.  Memory-constrained
     // applications may wish to call this method to reduce memory usage.
     // Default implementation of Prune() does nothing.  Subclasses are strongly
     // encouraged to override the default implementation.  A future release of
     // leveldb may change Prune() to a pure abstract method.
     virtual void Prune() {}

     // Return an estimate of the combined charges of all elements stored in the
     // cache.
     virtual size_t TotalCharge() const = 0;
 };
}
#endif //MYLEVEDB_CACHE_H
