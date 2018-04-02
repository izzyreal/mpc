#pragma once
#include <audio/mixer/MixerInterconnection.hpp>
#include <audio/server/AudioServer.hpp>

#include <memory>

namespace mpc {
	namespace ctootextensions {

		class MpcMixerInterconnection
			: public virtual ctoot::audio::mixer::MixerInterconnection
		{

		private:
			std::shared_ptr<ctoot::audio::core::AudioProcess> inputProcess;
			std::shared_ptr<ctoot::audio::core::AudioProcess> outputProcess;

		private:
			bool leftEnabled{ true };
			bool rightEnabled{ true };

		public:
			std::weak_ptr<ctoot::audio::core::AudioProcess> getInputProcess() override;
			std::weak_ptr<ctoot::audio::core::AudioProcess> getOutputProcess() override;
			
		public:
			void setLeftEnabled(bool b);
			void setRightEnabled(bool b);
			bool isLeftEnabled();
			bool isRightEnabled();

		public:
			MpcMixerInterconnection(std::string name, ctoot::audio::server::AudioServer* server);
			~MpcMixerInterconnection();

		};
	}
}
