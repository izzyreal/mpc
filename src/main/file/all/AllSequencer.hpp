#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::file::all
{
	class Sequencer
	{
	public:
		static const int LENGTH{ 16 };

	private:
		mpc::Mpc& mpc;
		static std::vector<char> TEMPLATE;

	public:
		static const int SEQ_OFFSET{ 0 };
		static const int TR_OFFSET{ 2 };
		static const int TC_OFFSET{ 7 };
		static const int SECOND_SEQ_ENABLED_OFFSET{ 9 };
		static const int SECOND_SEQ_INDEX_OFFSET{ 10 };
		int sequence;
		int track;
		int tc;
		bool secondSeqEnabled;
		int secondSeqIndex;
		std::vector<char> saveBytes;

	public:
		std::vector<char> getBytes();

		Sequencer(mpc::Mpc& mpc, const std::vector<char>& loadBytes);
		Sequencer(mpc::Mpc& mpc);

	};
}
