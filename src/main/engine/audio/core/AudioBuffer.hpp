#pragma once
#include <engine/audio/core/FloatSampleBuffer.hpp>

#include <string>

#include <memory>

namespace mpc::engine::audio::core
{
	class AudioBuffer : public FloatSampleBuffer
	{

	private:
		bool realTime = true;
		std::string name;

	public:
		std::string getName();

	public:
		bool isRealTime();
		void setRealTime(bool realTime);

	public:
		float square();

        void copyFrom(AudioBuffer* src);
		bool isSilent();

	public:
		AudioBuffer(std::string nameToUse, int channelCount, int sampleCount, float sampleRate);

	};
}
