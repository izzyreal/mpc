#include <stdio.h>
#include "Logger.hpp"
#include "audio/core/AudioFormat.hpp"
#include "Mpc.hpp"
#include <string>
#include <chrono>
#include <thread>

int main() {
	moduru::Logger::l.setPath("moduru.log");
	moduru::Logger::l.log("Yay!");
	auto af = new ctoot::audio::core::AudioFormat(44100, 16, 2, true, false);
	printf("Moduru test successful\n");
	printf("AudioFormat sample rate: %i\n", (int)(af->getSampleRate()));
	if (af->getSampleRate() == 44100) {
		printf("ctoot test successful\n");
	}
	else {
		printf("ctoot test failed\n");
	}
	auto mpc = new mpc::Mpc();
	mpc->init("rtaudio", 44100);
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	printf("\nmpc test successful\n");
	delete mpc;
	delete af;
	return 0;
}
