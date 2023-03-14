#include <engine/audio/core/ChannelFormat.hpp>
#include <engine/audio/core/ChannelFormat2.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <cstdint>

using namespace std;
using namespace mpc::engine::audio::core;

int ChannelFormat::mix(mpc::engine::audio::core::AudioBuffer* destBuffer, mpc::engine::audio::core::AudioBuffer* sourceBuffer, vector<float>& gain)
{
	auto doMix = destBuffer != sourceBuffer;
	auto snc = sourceBuffer->getChannelCount();
	auto dnc = destBuffer->getChannelCount();
	if (dnc > 4 && snc != dnc) dnc = 4;

	auto ns = destBuffer->getSampleCount();
	float g;
	auto k = static_cast<float>((snc)) / dnc;
	for (auto i = 0; i < dnc; i++) {
		g = gain[i] * k;
		auto& in = sourceBuffer->getChannel(i % snc);
		auto& out = destBuffer->getChannel(i);
		if (doMix) {
			for (auto s = 0; s < ns; s++) {
				out[s] += in[s] * g;
			}
		}
		else {
			for (auto s = 0; s < ns; s++) {
				out[s] = in[s] * g;
			}
		}
	}
	auto ret = 1;
	if (!doMix) ret |= 2;
	return ret;
}

shared_ptr<ChannelFormat> ChannelFormat::STEREO() {
	static auto stereoChannelFormat = make_shared<ChannelFormat2>();
	return stereoChannelFormat;
}
