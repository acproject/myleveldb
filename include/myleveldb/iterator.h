//
// Created by qinhangyu on 23-8-22.
//

#ifndef MYLEVEDB_ITERATOR_H
#define MYLEVEDB_ITERATOR_H

#include "export.h"
#include "slice.h"
#include "status.h"

namespace myleveldb {
    class LEVELDB_EXPORT Iterator {
    public:
        Iterator();

        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator) = delete;

        virtual ~Iterator();

        virtual bool Valid() const = 0;

        virtual void SeekToFirst() = 0;

        virtual void SeekToLast() = 0;

        virtual void Seek(const Slice& targete) = 0;

        virtual void Next() = 0;

        virtual void Prev() = 0;

        virtual Slice key() const = 0;

        virtual Slice value() const = 0;

        virtual Status status() const = 0;

        using CleanupFunction = void (*)(void* arg1, void* arg2);
        void RegisterCleanup(CleanupFunction function, void* arg1, void* arg2);

    private:
        struct CleanupNode {
            bool IsEmpty() const { return function == nullptr; }

            void Run() {
                assert(function != nullptr);
                (*function)(arg1, arg2);
            }

            CleanupFunction  function;
            void* arg1;
            void* arg2;
            CleanupNode* next;
        };
        CleanupNode cleanup_head_;

    };

    LEVELDB_EXPORT Iterator* NewEmptyIterator();
    LEVELDB_EXPORT Iterator* NewErrorIterator(const Status& status);
} // namespace myleveldb

#endif //MYLEVEDB_ITERATOR_H
