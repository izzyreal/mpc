#pragma once

#include "Paths.hpp"

namespace mpc
{
    class TestPaths : public Paths
    {

    protected:
        fs::path appConfigHome() const override;

    public:
        fs::path appDocumentsPath() const override;
    };
} // namespace mpc