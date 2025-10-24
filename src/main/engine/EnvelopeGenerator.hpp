#pragma once

#include "EnvelopeControls.hpp"

namespace mpc::engine
{

    class EnvelopeGenerator
    {
    public:
        enum State
        {
            ATTACK,
            HOLD,
            DECAY,
            COMPLETE
        };

    private:
        State state{};
        float envelope{0};
        int holdCounter{0};
        EnvelopeControls *vars{nullptr};

    public:
        float getEnvelope(bool decay);

        bool isComplete();

        void reset();

        EnvelopeGenerator(EnvelopeControls *vars);
    };

} // namespace mpc::engine
