#pragma once

namespace mpc::engine::filter
{

    class StateVariableFilterElement
    {

    private:
        float prev{0.f};
        float low{0.f}, high{0.f}, band{0.f}, notch{0.f};

    public:
        bool bp{false};
        float mix{0.f};

    public:
        float filter(float in, float freq, float damp);
    };

} // namespace mpc::engine::filter
