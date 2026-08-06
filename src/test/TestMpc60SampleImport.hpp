#pragma once

#include "file/kaitai/Mpc60SampleImport.hpp"

#include <optional>

namespace mpc::test
{
    class ScopedMpc60ImportSetting
    {
    public:
        explicit ScopedMpc60ImportSetting(
            const std::optional<bool> enabled)
        {
            set(enabled);
        }

        ~ScopedMpc60ImportSetting()
        {
            mpc::file::kaitai::Mpc60SampleImportPolicy::setRuntimeOverride(
                std::nullopt);
        }

        ScopedMpc60ImportSetting(const ScopedMpc60ImportSetting &) = delete;
        ScopedMpc60ImportSetting &operator=(
            const ScopedMpc60ImportSetting &) = delete;

        void set(const std::optional<bool> enabled)
        {
            mpc::file::kaitai::Mpc60SampleImportPolicy::setRuntimeOverride(
                enabled);
        }
    };
} // namespace mpc::test
