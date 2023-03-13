#pragma once

#include <engine/control/CompoundControl.hpp>

#include <memory>
#include <string>

namespace ctoot {

    namespace control {
        class ControlLaw;

        class LawControl;
    }

    namespace synth::modules::filter {

        class FilterControls
                : public ctoot::control::CompoundControl
        {

        private:
            static const int FREQUENCY{ 0 };
            static const int RESONANCE{ 1 };

            ctoot::control::LawControl *cutoffControl{nullptr};
            ctoot::control::LawControl *resonanceControl{nullptr};
            float cutoff{0.f}, resonance{0.f};

        protected:
            int idOffset{0};

        private:
            int sampleRate{44100};

        public:
            void derive(ctoot::control::Control *c) override;

            virtual void createControls();

            virtual void deriveSampleRateIndependentVariables();

            virtual float deriveResonance();

            virtual float deriveCutoff();

            virtual ctoot::control::LawControl *createCutoffControl();

            virtual ctoot::control::LawControl *createResonanceControl();

        public:
            float getCutoff();

            float getResonance();

        public:
            FilterControls(int id, std::string name, int idOffset);

        private:
            static std::shared_ptr<ctoot::control::ControlLaw> SEMITONE_LAW();

        };

    }
}
