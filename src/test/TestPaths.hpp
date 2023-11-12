#pragma once

#include "Paths.hpp"

namespace mpc {
    class TestPaths : public Paths {
        fs::path appDocumentsPath() override;
    };
}