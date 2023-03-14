#pragma once
#include <engine/audio/core/FloatSampleBuffer.hpp>

#include <string>

#include <memory>

namespace mpc::engine::audio::core
{
	class ChannelFormat;

	class AudioBuffer
		: public FloatSampleBuffer
	{

	private:
		std::shared_ptr<ChannelFormat> channelFormat;
		bool realTime{ true };
		std::string name;

	public:
		std::string getName();

	protected:
		void setChannelCount(int count);

	public:
		bool isRealTime();
		void setRealTime(bool realTime);

	public:
		std::shared_ptr<ChannelFormat> guessFormat();

	public:
		std::shared_ptr<ChannelFormat> getChannelFormat();
		void setChannelFormat(std::shared_ptr<ChannelFormat> format);

        void swap(int a, int b);
		float square();

        void copyFrom(AudioBuffer* src);
		const bool isSilent();

	public:
		AudioBuffer(std::string name, int channelCount, int sampleCount, float sampleRate);

	};
}
