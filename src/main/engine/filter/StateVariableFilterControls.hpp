#pragma once

#include "FilterControls.hpp"

namespace mpc::engine::filter
{

    class StateVariableFilterControls : public FilterControls
    {
    public:
        void derive(Control *c) override;

        void deriveSampleRateIndependentVariables() override;

        float deriveResonance() override;

        StateVariableFilterControls(const std::string &name, int idOffset);
    };

} // namespace mpc::engine::filter
