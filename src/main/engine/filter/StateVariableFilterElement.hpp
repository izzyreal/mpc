#pragma once

namespace mpc::engine::filter
{
    class StateVariableFilterElement
    {
        double low = 0;
        double band = 0;

    public:
        float filter(float input, double coefficient, double damping);

        void resetState();
    };

} // namespace mpc::engine::filter
