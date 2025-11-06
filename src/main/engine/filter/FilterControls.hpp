#pragma once

#include "engine/control/CompoundControl.hpp"

#include <memory>
#include <string>

namespace mpc::engine::control
{
    class ControlLaw;

    class LawControl;
} // namespace mpc::engine::control

namespace mpc::engine::filter
{

    class FilterControls : public mpc::engine::control::CompoundControl
    {

    private:
        static const int FREQUENCY{0};
        static const int RESONANCE{1};

        mpc::engine::control::LawControl *cutoffControl{nullptr};
        mpc::engine::control::LawControl *resonanceControl{nullptr};
        float cutoff{0.f}, resonance{0.f};

    protected:
        int idOffset{0};

    private:
        int sampleRate{44100};

    public:
        void derive(mpc::engine::control::Control *c) override;

        virtual void createControls();

        virtual void deriveSampleRateIndependentVariables();

        virtual float deriveResonance();

        virtual float deriveCutoff();

        virtual mpc::engine::control::LawControl *createCutoffControl();

        virtual mpc::engine::control::LawControl *createResonanceControl();

    public:
        float getCutoff() const;

        float getResonance() const;

    public:
        FilterControls(int id, const std::string &name, int idOffset);

    private:
        static std::shared_ptr<mpc::engine::control::ControlLaw> SEMITONE_LAW();
    };
} // namespace mpc::engine::filter
