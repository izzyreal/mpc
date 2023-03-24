#pragma once

#include <engine/control/Control.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include <engine/util/any.hpp>

namespace mpc::engine::control {
    class EnumControl : public Control
    {
    private:
        nonstd::any value;

    public:
        virtual void setValue(nonstd::any value);

        virtual nonstd::any getValue();

    public:
        std::string getValueString() override;
        EnumControl(int id, std::string name, nonstd::any value);

    };

}
