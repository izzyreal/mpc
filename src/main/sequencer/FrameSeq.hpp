#pragma once

#include "FrameSeqBase.hpp"
#include <engine/audio/server/AudioClient.hpp>

namespace mpc::sequencer {

	class FrameSeq final : public FrameSeqBase, public ctoot::audio::server::AudioClient
	{
    public:
        explicit FrameSeq(mpc::Mpc& mpc);

        // AudioClient implementation
		void work(int nFrames) override;
    };
}
