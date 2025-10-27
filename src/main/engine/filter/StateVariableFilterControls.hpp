#pragma once

#include "FilterControls.hpp"

namespace mpc::engine::filter
{

    class StateVariableFilterControls : public FilterControls
    {
    public:
        void derive(mpc::engine::control::Control *c) override;

        void deriveSampleRateIndependentVariables() override;

        float deriveResonance() override;

    public:
        StateVariableFilterControls(std::string name, int idOffset);
    };

} // namespace mpc::engine::filter
