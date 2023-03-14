#pragma once

#include <engine/control/Control.hpp>

namespace mpc::engine::control {
    class BooleanControl : public Control
    {

    private:
        bool value{false};

    public:
        void setValue(bool value);

        bool getValue();

        int getIntValue() override;

    public:
        BooleanControl(int id, std::string name, bool initialValue);

    };

}
