#pragma once
#include "engine/control/CompoundControl.hpp"
#include "engine/control/LawControl.hpp"

namespace mpc::engine
{

    class EnvelopeControls : public mpc::engine::control::CompoundControl
    {

    private:
        static const int ATTACK{1};
        static const int HOLD{2};
        static const int DECAY{3};
        static const int RELEASE{5};

        mpc::engine::control::LawControl *attackControl{nullptr};
        mpc::engine::control::LawControl *holdControl{nullptr};
        mpc::engine::control::LawControl *decayControl{nullptr};
        float sampleRate{44100};
        float attack{}, decay{}, hold{0};
        int idOffset{0};

    public:
        void derive(mpc::engine::control::Control *c) override;
        void createControls();
        void deriveSampleRateIndependentVariables();
        void deriveSampleRateDependentVariables();
        float deriveHold() const;
        void setSampleRate(int rate);

    private:
        static float LOG_0_01_;

    private:
        static std::shared_ptr<mpc::engine::control::ControlLaw> ATTACK_LAW();
        static std::shared_ptr<mpc::engine::control::ControlLaw> HOLD_LAW();
        static std::shared_ptr<mpc::engine::control::ControlLaw> DECAY_LAW();

    public:
        float deriveTimeFactor(float milliseconds) const;
        float deriveAttack() const;
        float deriveDecay() const;
        mpc::engine::control::LawControl *createAttackControl(float init) const;
        mpc::engine::control::LawControl *createHoldControl(float init) const;
        mpc::engine::control::LawControl *createDecayControl(float init) const;

    public:
        float getAttackCoeff() const;
        float getHold() const;
        float getDecayCoeff() const;

    public:
        EnvelopeControls(int id, const std::string &name, int idOffset);
    };
} // namespace mpc::engine
