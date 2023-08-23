//
// Created by qinhangyu on 23-8-23.
//

#ifndef MYLEVEDB_TABLE_BUILDER_H
#define MYLEVEDB_TABLE_BUILDER_H
#include <cstdint>
#include "export.h"
#include "options.h"
#include "status.h"

namespace myleveldb {
    class BlockBuilder;
    class BlockHandle;
    class WritableFile;

    class LEVELDB_EXPORT TableBuilder {
    public:
        TableBuilder(const Options& options, WritableFile* file);
        TableBuilder(const TableBuilder&) = delete;
        TableBuilder& operator=(const TableBuilder&) = delete;

        // REQUIRES: Either Finish() or Abandon() has been called.
        ~TableBuilder();

        Status ChangeOptions(const Options& options);

        // Add key,value to the table being constructed.
        // REQUIRES: key is after any previously added key according to comparator.
        // REQUIRES: Finish(), Abandon() have not been called
        void Add(const Slice& key, const Slice& value);

        // Advanced operation: flush any buffered key/value pairs to file.
        // Can be used to ensure that two adjacent entries never live in
        // the same data block.  Most clients should not need to use this method.
        // REQUIRES: Finish(), Abandon() have not been called
        void Flush();

        // Return non-ok iff some error has been detected.
        Status status() const;

        // Finish building the table.  Stops using the file passed to the
        // constructor after this function returns.
        // REQUIRES: Finish(), Abandon() have not been called
        Status Finish();

        // Indicate that the contents of this builder should be abandoned.  Stops
        // using the file passed to the constructor after this function returns.
        // If the caller is not going to call Finish(), it must call Abandon()
        // before destroying this builder.
        // REQUIRES: Finish(), Abandon() have not been called
        void Abandon();

        // Number of calls to Add() so far.
        uint64_t NumEntries() const;

        // Size of the file generated so far.  If invoked after a successful
        // Finish() call, returns the size of the final generated file.
        uint64_t FileSize() const;

    private:
        bool ok() const { return status().ok(); }
        void WriteBlock(BlockBuilder* block, BlockHandle* handle);
        void WriteRawBlock(const Slice& data, CompressionType, BlockHandle* handle);

        struct Rep;
        Rep* rep_;

    };
}
#endif //MYLEVEDB_TABLE_BUILDER_H
