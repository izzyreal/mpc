#pragma once

#include "engine/control/Control.hpp"

#include <atomic>

namespace mpc::engine::control
{
    class BooleanControl final : public Control
    {
        std::atomic<bool> value{false};

    public:
        void setValue(bool value);

        bool getValue() const;

        BooleanControl(int id, const std::string &name);
    };

} // namespace mpc::engine::control
