#pragma once
#include <ctootextensions/MpcEnvelopeControls.hpp>

namespace mpc {
	namespace ctootextensions {

		class MpcEnvelopeGenerator
		{
		public:
			enum State { ATTACK, HOLD, DECAY, COMPLETE };

		private:
			State state{};
			float envelope{ 0 };
			int holdCounter{ 0 };
			MpcEnvelopeControls* vars{ nullptr };

		public:
			float getEnvelope(bool decay);
			bool isComplete();
			void reset();

			MpcEnvelopeGenerator(MpcEnvelopeControls* vars);

			~MpcEnvelopeGenerator();

		};

	}
}
