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

    class FilterControls : public control::CompoundControl
    {

        static const int FREQUENCY{0};
        static const int RESONANCE{1};

        control::LawControl *cutoffControl{nullptr};
        control::LawControl *resonanceControl{nullptr};
        float cutoff{0.f}, resonance{0.f};

    protected:
        int idOffset{0};

    private:
        int sampleRate{44100};

    public:
        void derive(Control *c) override;

        virtual void createControls();

        virtual void deriveSampleRateIndependentVariables();

        virtual float deriveResonance();

        virtual float deriveCutoff();

        virtual control::LawControl *createCutoffControl();

        virtual control::LawControl *createResonanceControl();

        float getCutoff() const;

        float getResonance() const;

        FilterControls(int id, const std::string &name, int idOffset);

    private:
        static std::shared_ptr<control::ControlLaw> SEMITONE_LAW();
    };
} // namespace mpc::engine::filter
