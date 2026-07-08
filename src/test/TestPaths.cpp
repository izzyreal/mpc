#include "TestPaths.hpp"

#include <atomic>
#include <chrono>
#include <string>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

namespace
{
    int current_process_id()
    {
#ifdef _WIN32
        return _getpid();
#else
        return getpid();
#endif
    }

    mpc_fs::path make_unique_test_documents_path()
    {
        static std::atomic<uint64_t> counter{0};
        const auto serial = counter.fetch_add(1, std::memory_order_relaxed);
        const auto ticks =
            std::chrono::steady_clock::now().time_since_epoch().count();

        return mpc_fs::temp_directory_path().value_or(mpc_fs::path{}) /
               ("VMPC2000XL-test-" + std::to_string(current_process_id()) +
                "-" + std::to_string(serial) + "-" + std::to_string(ticks));
    }
} // namespace

mpc::TestPaths::TestPaths()
    : testDocumentsPath(make_unique_test_documents_path())
{
}

mpc_fs::path mpc::TestPaths::appConfigHome() const
{
    return appDocumentsPath() / "config";
}

mpc_fs::path mpc::TestPaths::appDocumentsPath() const
{
    return testDocumentsPath;
}
