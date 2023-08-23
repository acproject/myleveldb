//
// Created by qinhangyu on 23-8-23.
//

#ifndef MYLEVEDB_ENV_H
#define MYLEVEDB_ENV_H
#include "export.h"
#include "status.h"
#include <cstdarg>
#include <cstdint>
#include <string>
#include <vector>

#if defined(_WIN32)

#if defined(DeleteFile)
#undef  DeleteFile
#define LEVELDB_DELETEFILE_UNDEFINED
#endif // defined(DeleteFile)
#endif // defined(_WIN32)

namespace myleveldb {
    class FileLock;
    class Logger;
    class RandomAccessFile;
    class SequentialFile;
    class Slice;
    class WritableFile;

    class LEVELDB_EXPORT Env {
    public:
        Env();
        Env(const Env&) = delete;
        Env& operator=(const Env&) = delete;

        virtual ~Env();

        static Env* Default();

        virtual Status NewSequentialFile(const std::string& fname, SequentialFile** result) = 0;

        virtual Status NewRandomAccessFile(const std::string& fname, RandomAccessFile** result) = 0;
        virtual Status NewAppendableFile(const std::string& fname,
                                         WritableFile** result);
        virtual Status NewWritableFile(const std::string& fname,
                                       WritableFile** result) = 0;
        virtual bool FileExists(const std::string& fname) = 0;

        virtual Status GetChildren(const std::string& dir,
                                   std::vector<std::string>* result) = 0;

        virtual Status RemoveFile(const std::string& fname);

        virtual Status DeleteFile(const std::string& fname);

        virtual Status CreateDir(const std::string& dirname) = 0;

        virtual Status RemoveDir(const std::string& dirname);

        virtual Status DeleteDir(const std::string& dirname);

        virtual Status GetFileSize(const std::string& fname, uint64_t* file_size) = 0;

        virtual Status RenameFile(const std::string& src,
                                  const std::string& target) = 0;

        // Lock the specified file.  Used to prevent concurrent access to
        // the same db by multiple processes.  On failure, stores nullptr in
        // *lock and returns non-OK.
        //
        // On success, stores a pointer to the object that represents the
        // acquired lock in *lock and returns OK.  The caller should call
        // UnlockFile(*lock) to release the lock.  If the process exits,
        // the lock will be automatically released.
        //
        // If somebody else already holds the lock, finishes immediately
        // with a failure.  I.e., this call does not wait for existing locks
        // to go away.
        //
        // May create the named file if it does not already exist.
        virtual Status LockFile(const std::string& fname, FileLock** lock) = 0;

        // Release the lock acquired by a previous successful call to LockFile.
        // REQUIRES: lock was returned by a successful LockFile() call
        // REQUIRES: lock has not already been unlocked.
        virtual Status UnlockFile(FileLock* lock) = 0;

        // Arrange to run "(*function)(arg)" once in a background thread.
        //
        // "function" may run in an unspecified thread.  Multiple functions
        // added to the same Env may run concurrently in different threads.
        // I.e., the caller may not assume that background work items are
        // serialized.
        virtual void Schedule(void (*function)(void* arg), void* arg) = 0;

        // Start a new thread, invoking "function(arg)" within the new thread.
        // When "function(arg)" returns, the thread will be destroyed.
        virtual void StartThread(void (*function)(void* arg), void* arg) = 0;

        // *path is set to a temporary directory that can be used for testing. It may
        // or may not have just been created. The directory may or may not differ
        // between runs of the same process, but subsequent calls will return the
        // same directory.
        virtual Status GetTestDirectory(std::string* path) = 0;

        // Create and return a log file for storing informational messages.
        virtual Status NewLogger(const std::string& fname, Logger** result) = 0;

        // Returns the number of micro-seconds since some fixed point in time. Only
        // useful for computing deltas of time.
        virtual uint64_t NowMicros() = 0;

        // Sleep/delay the thread for the prescribed number of micro-seconds.
        virtual void SleepForMicroseconds(int micros) = 0;
    };

// A file abstraction for reading sequentially through a file
    class LEVELDB_EXPORT SequentialFile {
    public:
        SequentialFile() = default;

        SequentialFile(const SequentialFile&) = delete;
        SequentialFile& operator=(const SequentialFile&) = delete;

        virtual ~SequentialFile();

        // Read up to "n" bytes from the file.  "scratch[0..n-1]" may be
        // written by this routine.  Sets "*result" to the data that was
        // read (including if fewer than "n" bytes were successfully read).
        // May set "*result" to point at data in "scratch[0..n-1]", so
        // "scratch[0..n-1]" must be live when "*result" is used.
        // If an error was encountered, returns a non-OK status.
        //
        // REQUIRES: External synchronization
        virtual Status Read(size_t n, Slice* result, char* scratch) = 0;

        // Skip "n" bytes from the file. This is guaranteed to be no
        // slower that reading the same data, but may be faster.
        //
        // If end of file is reached, skipping will stop at the end of the
        // file, and Skip will return OK.
        //
        // REQUIRES: External synchronization
        virtual Status Skip(uint64_t n) = 0;
    };

    class LEVELDB_EXPORT WritableFile {
    public:
        WritableFile() = default;

        WritableFile(const WritableFile&) = delete;
        WritableFile& operator=(const WritableFile&) = delete;

        virtual ~WritableFile();
        virtual Status Append(const Slice& data) = 0;
        virtual Status Close() = 0;
        virtual Status Flush() = 0;
        virtual Status Sync() = 0;

    };

    class LEVELDB_EXPORT Logger {
    public:
        Logger() = default;

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        virtual ~Logger();

        // Write an entry to the log file with the specified format.
        virtual void Logv(const char* format, std::va_list ap) = 0;
    };

    class LEVELDB_EXPORT FileLock {
    public:
        FileLock() = default;

        FileLock(const FileLock&) = delete;
        FileLock& operator=(const FileLock&) = delete;

        virtual ~FileLock();
    };

    void Log(Logger* info_log, const char* format, ...)

    #if defined(__GNUC__) || defined(__clang__)
        __attribute__((__format__(__printf__, 2, 3)))
    #endif
    ;

    // A utility routine: write "data" to the named file.
    LEVELDB_EXPORT Status WriteStringToFile(Env* env, const Slice& data,
                                            const std::string& fname);

// A utility routine: read contents of named file into *data
    LEVELDB_EXPORT Status ReadFileToString(Env* env, const std::string& fname,
                                           std::string* data);

    class LEVELDB_EXPORT EnvWrapper : public Env {
    public:
        // Initialize an EnvWrapper that delegates all calls to *t.
        explicit EnvWrapper(Env* t) : target_(t) {}
        virtual ~EnvWrapper();

        // Return the target to which this Env forwards all calls.
        Env* target() const { return target_; }

        // The following text is boilerplate that forwards all methods to target().
        Status NewSequentialFile(const std::string& f, SequentialFile** r) override {
            return target_->NewSequentialFile(f, r);
        }
        Status NewRandomAccessFile(const std::string& f,
                                   RandomAccessFile** r) override {
            return target_->NewRandomAccessFile(f, r);
        }
        Status NewWritableFile(const std::string& f, WritableFile** r) override {
            return target_->NewWritableFile(f, r);
        }
        Status NewAppendableFile(const std::string& f, WritableFile** r) override {
            return target_->NewAppendableFile(f, r);
        }
        bool FileExists(const std::string& f) override {
            return target_->FileExists(f);
        }
        Status GetChildren(const std::string& dir,
                           std::vector<std::string>* r) override {
            return target_->GetChildren(dir, r);
        }
        Status RemoveFile(const std::string& f) override {
            return target_->RemoveFile(f);
        }
        Status CreateDir(const std::string& d) override {
            return target_->CreateDir(d);
        }
        Status RemoveDir(const std::string& d) override {
            return target_->RemoveDir(d);
        }
        Status GetFileSize(const std::string& f, uint64_t* s) override {
            return target_->GetFileSize(f, s);
        }
        Status RenameFile(const std::string& s, const std::string& t) override {
            return target_->RenameFile(s, t);
        }
        Status LockFile(const std::string& f, FileLock** l) override {
            return target_->LockFile(f, l);
        }
        Status UnlockFile(FileLock* l) override { return target_->UnlockFile(l); }
        void Schedule(void (*f)(void*), void* a) override {
            return target_->Schedule(f, a);
        }
        void StartThread(void (*f)(void*), void* a) override {
            return target_->StartThread(f, a);
        }
        Status GetTestDirectory(std::string* path) override {
            return target_->GetTestDirectory(path);
        }
        Status NewLogger(const std::string& fname, Logger** result) override {
            return target_->NewLogger(fname, result);
        }
        uint64_t NowMicros() override { return target_->NowMicros(); }
        void SleepForMicroseconds(int micros) override {
            target_->SleepForMicroseconds(micros);
        }

    private:
        Env* target_;
    };
}

#if defined(_WIN32) && defined(LEVELDB_DELETEFILE_UNDEFINED)
#if defined(UNICODE)
#define  DeleteFile DeleteFileW
#else
#define DeleteFile DeleteFileA
#endif // defined(UNICODE)
#endif // defined(_WIN32) && defined(LEVELDB_DELETEFILE_UNDEFINED)

#endif //MYLEVEDB_ENV_H
