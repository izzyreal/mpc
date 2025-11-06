#pragma once

#include "engine/control/Control.hpp"

namespace mpc::engine::control
{
    class BooleanControl : public Control
    {

    private:
        bool value{false};

    public:
        void setValue(bool value);

        bool getValue() const;

    public:
        BooleanControl(int id, const std::string &name);
    };

} // namespace mpc::engine::control
