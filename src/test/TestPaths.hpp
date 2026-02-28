#pragma once

#include "Paths.hpp"

namespace mpc
{
    class TestPaths : public Paths
    {

    protected:
        mpc_fs::path appConfigHome() const override;

    public:
        mpc_fs::path appDocumentsPath() const override;
    };
} // namespace mpc