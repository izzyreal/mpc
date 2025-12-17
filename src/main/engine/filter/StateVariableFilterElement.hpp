#pragma once

namespace mpc::engine::filter
{
    class StateVariableFilterElement
    {
        float prev, low, high, band, notch;

    public:
        StateVariableFilterElement();

        float filter(float in, float freq, float damp);

        void resetState();
    };

} // namespace mpc::engine::filter
