//
// Created by qinhangyu on 23-8-22.
//

#ifndef MYLEVEDB_STATUS_H
#define MYLEVEDB_STATUS_H
#include "export.h"
#include "slice.h"
#include <algorithm>
#include <string>

namespace myleveldb {
    class LEVELDB_EXPORT Status {
    public:
        Status() noexcept: state_(nullptr) {}
        ~Status() { delete[] state_;}

        Status(const Status& rhs);
        Status& operator=(const Status& rhs);

        Status(Status&& mrhs) noexcept: state_(mrhs.state_) {
            mrhs.state_ = nullptr;
        }
        Status& operator=(Status&& mrhs) noexcept;

        static Status OK() {return  Status(); }

        static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
            return Status(kCorruption, msg, msg2);
        }

        static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
            return Status(kNotSupported, msg, msg2);
        }

        static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
            return Status(kInvalidArgument, msg, msg2);
        }

        static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
            return Status(kIOError, msg, msg2);
        }

        bool  ok() const { return  (state_ == nullptr); }

        bool IsNotFound() const { return  code() == kNotFound; }

        bool IsCorruption() const { return  code() == kCorruption; }

        bool IsIOError() const {return  code() == kIOError; }

        bool IsNotSupportedError() const { return  code() == kNotSupported; }

        bool IsInvalidArgument() const { return code() == kInvalidArgument; }

        std::string ToString() const;

    private:
        enum Code {
            kOk = 0,
            kNotFound = 1,
            kCorruption = 2,
            kNotSupported = 3,
            kInvalidArgument = 4,
            kIOError = 5
        };

        Code code() const {
            return (state_ == nullptr) ? kOk : static_cast<Code>(state_[4]);
        }
        Status(Code code, const Slice& msg, const Slice& msg2);
        static const char* CopyState(const char* s);

        // OK status has a null state_.  Otherwise, state_ is a new[] array
        // of the following form:
        //    state_[0..3] == length of message
        //    state_[4]    == code
        //    state_[5..]  == message
        const char* state_;
    };

    inline Status::Status(const Status& rhs) {
        state_ = (rhs.state_ == nullptr) ? nullptr : CopyState(rhs.state_);
    }

    inline Status& Status::operator=(const myleveldb::Status &rhs) {
        if (state_ != rhs.state_) {
            delete[] state_;
            state_ = (rhs.state_ == nullptr) ? nullptr : CopyState(rhs.state_);
        }
        return *this;
    }

    inline Status& Status::operator=(myleveldb::Status &&mrhs) noexcept {
        std::swap(state_, mrhs.state_);
        return *this;
    }
} // namesapce myleveldb
#endif //MYLEVEDB_STATUS_H
