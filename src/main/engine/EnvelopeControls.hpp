#pragma once
#include "engine/control/CompoundControl.hpp"
#include "engine/control/LawControl.hpp"

namespace mpc::engine
{

    class EnvelopeControls : public control::CompoundControl
    {

        static const int ATTACK{1};
        static const int HOLD{2};
        static const int DECAY{3};
        static const int RELEASE{5};

        control::LawControl *attackControl{nullptr};
        control::LawControl *holdControl{nullptr};
        control::LawControl *decayControl{nullptr};
        float sampleRate{44100};
        float attack{}, decay{}, hold{0};
        int idOffset{0};

    public:
        void derive(Control *c) override;
        void createControls();
        void deriveSampleRateIndependentVariables();
        void deriveSampleRateDependentVariables();
        float deriveHold() const;
        void setSampleRate(int rate);

    private:
        static float LOG_0_01_;

        static std::shared_ptr<control::ControlLaw> ATTACK_LAW();
        static std::shared_ptr<control::ControlLaw> HOLD_LAW();
        static std::shared_ptr<control::ControlLaw> DECAY_LAW();

    public:
        float deriveTimeFactor(float milliseconds) const;
        float deriveAttack() const;
        float deriveDecay() const;
        control::LawControl *createAttackControl(float init) const;
        control::LawControl *createHoldControl(float init) const;
        control::LawControl *createDecayControl(float init) const;

        float getAttackCoeff() const;
        float getHold() const;
        float getDecayCoeff() const;

        EnvelopeControls(int id, const std::string &name, int idOffset);
    };
} // namespace mpc::engine
