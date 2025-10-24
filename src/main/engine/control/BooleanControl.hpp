#pragma once

#include <engine/control/Control.hpp>

namespace mpc::engine::control
{
    class BooleanControl : public Control
    {

    private:
        bool value{false};

    public:
        void setValue(bool value);

        bool getValue();

    public:
        BooleanControl(int id, std::string name);
    };

} // namespace mpc::engine::control
