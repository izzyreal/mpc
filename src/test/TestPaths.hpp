#pragma once

#include "Paths.hpp"

namespace mpc
{
    class TestPaths : public Paths
    {
        mpc_fs::path testDocumentsPath;

    protected:
        mpc_fs::path appConfigHome() const override;

    public:
        TestPaths();
        mpc_fs::path appDocumentsPath() const override;
    };
} // namespace mpc
