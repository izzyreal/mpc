#pragma once

#include <memory>
#include <vector>
#include <thread>

namespace mpc {

	

	namespace ui {

		namespace disk {
			class DiskGui;
		}
	}

	namespace sampler {
		class Sound;
	}

	namespace disk {

		class AbstractDisk;

		class SoundSaver
		{

		public:
			
			std::weak_ptr<AbstractDisk> disk{};
			mpc::ui::disk::DiskGui* diskGui{};

		private:
			std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds{};
			bool wav{ false };

		private:
			std::thread saveSoundsThread{};
			static void static_saveSounds(void* this_p);
			void saveSounds();

		public:
			SoundSaver(std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds, bool wav);
			~SoundSaver();

		};

	}
}
