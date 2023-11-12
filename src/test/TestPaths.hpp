#pragma once

#include "Paths.hpp"

namespace mpc {
    class TestPaths : public Paths {

    protected:
        fs::path appConfigHome() override;

    public:
        fs::path appDocumentsPath() override;
    };
}