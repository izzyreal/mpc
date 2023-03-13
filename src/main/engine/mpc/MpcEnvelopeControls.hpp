#pragma once
#include <engine/control/CompoundControl.hpp>
#include <engine/control/LawControl.hpp>

namespace ctoot::mpc {

		class MpcEnvelopeControls
			: public ctoot::control::CompoundControl
		{

		private:
            static const int ATTACK{1};
            static const int HOLD{2};
            static const int DECAY{3};
            static const int RELEASE{5};

            ctoot::control::LawControl* attackControl{ nullptr };
			ctoot::control::LawControl* holdControl{ nullptr };
			ctoot::control::LawControl* decayControl{ nullptr };
			float sampleRate{ 44100 };
			float attack{}, decay{}, hold{ 0 };
			int idOffset{ 0 };

		public:
			void derive(ctoot::control::Control* c) override;
			void createControls();
			void deriveSampleRateIndependentVariables();
			void deriveSampleRateDependentVariables();
			float deriveHold();
			void setSampleRate(int rate);

		private:
			static float LOG_0_01_;

		private:
			static std::shared_ptr<ctoot::control::ControlLaw> ATTACK_LAW();
			static std::shared_ptr<ctoot::control::ControlLaw> HOLD_LAW();
			static std::shared_ptr<ctoot::control::ControlLaw> DECAY_LAW();

		public:
			float deriveTimeFactor(float milliseconds);
			float deriveAttack();
			float deriveDecay();
			ctoot::control::LawControl* createAttackControl(float init);
			ctoot::control::LawControl* createHoldControl(float init);
			ctoot::control::LawControl* createDecayControl(float init);

		public:
			float getAttackCoeff();
			float getHold();
			float getDecayCoeff();

		public:
			MpcEnvelopeControls(int id, std::string name, int idOffset);
        };
	}
