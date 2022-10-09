#pragma once

#include <memory>
#include <vector>

namespace mpc { class Mpc; }

namespace mpc::sampler
{
	class Pad
	{

	private:
		static std::vector<int> originalPadNotes;

	public:
        static std::vector<int> iRigPadsDefaultMapping;
		static std::vector<int>& getPadNotes(mpc::Mpc& mpc);

	private:
		mpc::Mpc& mpc;
		int note = 0;
		int index = 0;

	public:
		void setNote(int i);
		int getNote();
		int getIndex();

	public:
		Pad(mpc::Mpc& mpc, int index);

	};
}
