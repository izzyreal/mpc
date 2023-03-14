#pragma once
#include <engine/audio/server/AudioServer.hpp>

#include <memory>

namespace ctoot::mpc {

		class MixerInterconnection
		{

		private:
			std::shared_ptr<ctoot::audio::core::AudioProcess> inputProcess;
			std::shared_ptr<ctoot::audio::core::AudioProcess> outputProcess;

		private:
			bool leftEnabled{ true };
			bool rightEnabled{ true };

		public:
			std::shared_ptr<ctoot::audio::core::AudioProcess> getInputProcess();
			std::shared_ptr<ctoot::audio::core::AudioProcess> getOutputProcess();
			
		public:
			void setLeftEnabled(bool b);
			void setRightEnabled(bool b);
			bool isLeftEnabled();
			bool isRightEnabled();

		public:
			MixerInterconnection(std::string name, ctoot::audio::server::AudioServer* server);
			virtual ~MixerInterconnection();

		};
	}
