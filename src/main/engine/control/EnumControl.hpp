#pragma once

#include <engine/control/Control.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include <any.hpp>

namespace mpc::engine::control {
    class EnumControl : public Control
    {
    private:
        nonstd::any value;

    public:
        void setValue(nonstd::any value);

        std::string getValueString() override;
        EnumControl(int id, std::string name, nonstd::any value);

    };

}
