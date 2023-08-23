//
// Created by qinhangyu on 23-8-23.
//

#ifndef MYLEVEDB_FILTER_POLICY_H
#define MYLEVEDB_FILTER_POLICY_H
#include <string>
#include "export.h"
namespace myleveldb {
    class Slice;

    class LEVELDB_EXPORT FilterPolicy {
    public:
        virtual ~FilterPolicy();

        virtual const char* Name() const = 0;

        virtual void CreateFilter(const Slice* keys, int n,
                                  std::string* dst) const = 0;

        virtual bool KeyMayMatch(const Slice& key, const Slice& filter) const = 0;
    };

    LEVELDB_EXPORT const FilterPolicy* NewBloomFilterPolicy(int bits_per_key);
}
#endif //MYLEVEDB_FILTER_POLICY_H
