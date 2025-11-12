#pragma once

#include "engine/control/Control.hpp"

namespace mpc::engine::control
{
    class BooleanControl : public Control
    {
        bool value{false};

    public:
        void setValue(bool value);

        bool getValue() const;

        BooleanControl(int id, const std::string &name);
    };

} // namespace mpc::engine::control
