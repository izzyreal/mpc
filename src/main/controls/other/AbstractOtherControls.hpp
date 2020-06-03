#pragma once
#include <controls/BaseControls.hpp>

#include <memory>

namespace mpc::sequencer
{
	class Sequence;
}

namespace mpc::ui
{
	class UserDefaults;
}

namespace mpc::controls::other
{

	class AbstractOtherControls
		: public mpc::controls::BaseControls
	{

	protected:
		std::weak_ptr<mpc::sequencer::Sequence> sequence;

	protected:
		std::vector<int> tickValues;
		std::vector<char> akaiAsciiChar;
		int trackNum{ 0 };
		int seqNum{ 0 };

	protected:
		void init() override;

	public:
		AbstractOtherControls();

	};
}
