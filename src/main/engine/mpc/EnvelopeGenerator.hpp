#pragma once

#include <engine/mpc/EnvelopeControls.hpp>

namespace ctoot::mpc {

    class EnvelopeGenerator
    {
    public:
        enum State
        {
            ATTACK, HOLD, DECAY, COMPLETE
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

        ~EnvelopeGenerator();

    };

}
