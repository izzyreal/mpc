#pragma once

#include <memory>
#include <vector>

namespace ctoot::mpc {
	class MpcSound {

	public:
		virtual bool isMono() = 0;
		virtual int getTune() = 0;
		virtual int getStart() = 0;
		virtual int getLastFrameIndex() = 0;
		virtual int getEnd() = 0;
		virtual int getLoopTo() = 0;
		virtual std::vector<float>* getSampleData() = 0;
		virtual bool isLoopEnabled() = 0;
		virtual int getSndLevel() = 0;

	};
}
