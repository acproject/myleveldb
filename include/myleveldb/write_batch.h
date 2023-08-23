//
// Created by qinhangyu on 23-8-23.
//

#ifndef MYLEVEDB_WRITE_BATCH_H
#define MYLEVEDB_WRITE_BATCH_H
#include <string>

#include "export.h"
#include "status.h"
namespace myleveldb {
    class Slice;

    class LEVELDB_EXPORT WriteBatch {
    public:
        class LEVELDB_EXPORT Handler {
        public:
            virtual ~Handler();
            virtual void Put(const Slice& key, const Slice& value) = 0;
            virtual void Delete(const Slice& key) = 0;
        };

        WriteBatch();
        WriteBatch(const WriteBatch&) = default;
        WriteBatch& operator=(const WriteBatch&) =default;

        ~WriteBatch();

        void Put(const Slice& key, const Slice& value);

        // If the database contains a mapping for "key", erase it.  Else do nothing.
        void Delete(const Slice& key);

        // Clear all updates buffered in this batch.
        void Clear();

        size_t ApproximateSize() const;

        // Copies the operations in "source" to this batch.
        //
        // This runs in O(source size) time. However, the constant factor is better
        // than calling Iterate() over the source batch with a Handler that replicates
        // the operations into this batch.
        void Append(const WriteBatch& source);

        // Support for iterating over the contents of a batch.
        Status Iterate(Handler* handler) const;

    private:
        friend class WriteBatchInternal;

        std::string rep_;  // See comment in write_batch.cc for the format of rep_
    };
}



#endif //MYLEVEDB_WRITE_BATCH_H
