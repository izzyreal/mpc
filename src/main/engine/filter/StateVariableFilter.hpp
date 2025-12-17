#pragma once

#include "FilterControls.hpp"

namespace mpc::engine::filter
{

    class StateVariableFilterElement;

    class StateVariableFilter
    {
    public:
        explicit StateVariableFilter(FilterControls *variables);

        ~StateVariableFilter();

    private:
        StateVariableFilterElement *element{nullptr};
        float res{0.f};
        FilterControls *vars;
        float fs{44100.0};

        void setSampleRate(int rate);

    public:
        float filter(float sample, float cutoff, float resonance) const;
        void resetElementState() const;
    };
} // namespace mpc::engine::filter
