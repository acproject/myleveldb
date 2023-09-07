//
// Created by qinhangyu on 23-9-7.
//

#include "leveldb/c.h"

#include <string>
#include <cstdlib>
#include <cstdint>

#include "leveldb/cache.h"
#include "leveldb/comparator.h"
#include "leveldb//db.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/iterator.h"
#include "leveldb/options.h"
#include "leveldb/status.h"
#include "leveldb/write_batch.h"

using leveldb::Cache;
using leveldb::Comparator;
using leveldb::CompressionType;
using leveldb::DB;
using leveldb::Env;
using leveldb::FileLock;
using leveldb::FilterPolicy;
using leveldb::Iterator;
using leveldb::kMajorVersion;
using leveldb::kMinorVersion;
using leveldb::Logger;
using leveldb::NewBloomFilterPolicy;
using leveldb::NewLRUCache;
using leveldb::Options;
using leveldb::RandomAccessFile;
using leveldb::Range;
using leveldb::ReadOptions;
using leveldb::SequentialFile;
using leveldb::Slice;
using leveldb::Snapshot;
using leveldb::Status;
using leveldb::WritableFile;
using leveldb::WriteBatch;
using leveldb::WriteOptions;


extern "C" {
    struct leveldb_t {
        DB* rep;
    };
struct leveldb_iterator_t {
    Iterator* rep;
};
struct leveldb_writebatch_t {
    WriteBatch rep;
};
struct leveldb_snapshot_t {
    const Snapshot* rep;
};
struct leveldb_readoptions_t {
    ReadOptions rep;
};
struct leveldb_writeoptions_t {
    WriteOptions rep;
};
struct leveldb_options_t {
    Options rep;
};
struct leveldb_cache_t {
    Cache* rep;
};
struct leveldb_seqfile_t {
    SequentialFile* rep;
};
struct leveldb_randomfile_t {
    RandomAccessFile* rep;
};
struct leveldb_writablefile_t {
    WritableFile* rep;
};
struct leveldb_logger_t {
    Logger* rep;
};
struct leveldb_filelock_t {
    FileLock* rep;
};

struct leveldb_comparator_t: public Comparator {
    ~leveldb_comparator_t() override { (*destructor_)(state_); }


    int Compare(const Slice& a, const Slice& b) const override {
        return (*compare_)(state_, a.data(), a.size(), b.data(), b.size());
    }

    const char* Name() const override { return (*name_)(state_); }

    // No-ops since the C binding does not support key shortening methods.
    void FindShortestSeparator(std::string*, const Slice&) const override {}
    void FindShortSuccessor(std::string* key) const override {}
    void* state_;
    void (*destructor_)(void*);
    int (*compare_)(void*, const char* a, size_t alen, const char* b,
                    size_t blen);
    const char* (*name_)(void*);

};
}