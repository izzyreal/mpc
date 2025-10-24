#pragma once

#include <vector>

namespace mpc::engine
{
    class VoiceConstants
    {
    public:
        static const std::vector<float> EMPTY_FRAME;
        static const int SVF_OFFSET = 48;
        static const int AMPENV_OFFSET = 64;
        static constexpr float STATIC_ATTACK_LENGTH = 10.92f;
        static constexpr float STATIC_DECAY_LENGTH = 109.2f;
        static const int MAX_ATTACK_LENGTH_MS = 3000;
        static const int MAX_DECAY_LENGTH_MS = 2600;
        static const int MAX_ATTACK_LENGTH_SAMPLES = 132300;
        static const int MAX_DECAY_LENGTH_SAMPLES = 114660;
        static const int ATTACK_INDEX = 0;
        static const int HOLD_INDEX = 1;
        static const int DECAY_INDEX = 2;
        static const int RESO_INDEX = 1;
        static constexpr float ENV_TIME_RATIO = 5.46f;
    };

    inline const std::vector<float> VoiceConstants::EMPTY_FRAME{0, 0};
} // namespace mpc::engine
