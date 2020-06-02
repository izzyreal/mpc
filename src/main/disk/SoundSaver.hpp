#pragma once

#include <memory>
#include <vector>
#include <thread>

namespace mpc::sampler {
	class Sound;
}

namespace mpc::disk {
	class AbstractDisk;
}

namespace mpc::disk {

	class SoundSaver
	{

	private:
		std::weak_ptr<AbstractDisk> disk;
		std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds;
		bool wav = false;
		std::thread saveSoundsThread;
		static void static_saveSounds(void* this_p);
		void saveSounds();

	public:
		SoundSaver(std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds, bool wav);
		~SoundSaver();

	};
}
