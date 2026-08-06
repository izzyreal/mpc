#include "file/kaitai/Mpc60SampleImport.hpp"

#include <atomic>
#include <cstdlib>
#include <string_view>

namespace
{
    constexpr int kUseEnvironment = -1;
    constexpr int kDisabled = 0;
    constexpr int kEnabled = 1;

    std::atomic<int> runtimeOverride{kUseEnvironment};

    bool environmentDefault()
    {
        // Read once on first use. Changing the process environment afterward
        // has no effect; use setRuntimeOverride for in-process changes.
        static const bool enabled = []
        {
            const auto *value =
                std::getenv("VMPC2000XL_DISABLE_MPC60_SAMPLE_IMPORT");
            return value == nullptr || std::string_view(value) != "1";
        }();
        return enabled;
    }
} // namespace

bool mpc::file::kaitai::Mpc60SampleImportPolicy::isEnabled()
{
    const auto value = runtimeOverride.load(std::memory_order_relaxed);
    if (value == kUseEnvironment)
    {
        return environmentDefault();
    }
    return value == kEnabled;
}

void mpc::file::kaitai::Mpc60SampleImportPolicy::setRuntimeOverride(
    const std::optional<bool> enabled)
{
    runtimeOverride.store(enabled.has_value()
                              ? (*enabled ? kEnabled : kDisabled)
                              : kUseEnvironment,
                          std::memory_order_relaxed);
}
