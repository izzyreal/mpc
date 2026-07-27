#pragma once

namespace mpc::engine::filter
{

    class StateVariableFilterElement;

    class StateVariableFilter
    {
    public:
        StateVariableFilter();

        ~StateVariableFilter();

    private:
        StateVariableFilterElement *element{nullptr};

    public:
        float filter(float sample, double coefficient, int resonance) const;
        void resetElementState() const;
    };
} // namespace mpc::engine::filter
