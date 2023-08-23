//
// Created by qinhangyu on 23-8-23.
//

#ifndef MYLEVEDB_TABLE_H
#define MYLEVEDB_TABLE_H
#include "export.h"
#include "iterator.h"

#include <cstdint>

namespace myleveldb {
    class Block;
    class BlockHandle;
    class Footer;
    struct Options;
    class RandomAccessFile;
    struct ReadOptions;
    class TableCache;

    class LEVELDB_EXPORT Table {
    public:
        static Status Open(const Options& options, RandomAccessFile* file,
                           uint64_t file_size, Table** table);

        Table(const Table&) =delete;
        Table& operator=(const Table&) = delete;

        ~Table();

        Iterator* NewIterator(const ReadOptions&) const;

        uint64_t ApproximateOffsetOf(const Slice& key) const;

    private:
        friend class TableCache;
        struct Rep;

        static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);

        explicit Table(Rep* rep) : rep_(rep) {}

        /**
         * @param ReadOptions  参数是一个类型为ReadOptions的变量，用于指定读取操作的选项和配置。
         * @param key Slice的引用，用于指定要读取的键(key)。
         * @param arg 指向void类型的指针，用于传递给handle_result函数的回调参数。
         * @param handle_result 指向函数的指针，该函数接受一个void类型的指针参数和两个Slice类型的引用参数，用于处理读取结果。
         * @return
         */
        Status InternalGet(const ReadOptions, const Slice& key, void* arg,
                           void (*handle_result)(void* arg, const Slice& k, const Slice& v));

        void ReadMeta(const Footer& footer);
        void ReadFilter(const Slice& filter_handle_value);

        Rep* const rep_;
    };
}
#endif //MYLEVEDB_TABLE_H
