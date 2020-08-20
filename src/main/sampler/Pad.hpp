#pragma once

#include <observer/Observable.hpp>

#include <memory>

namespace mpc { class Mpc; }

namespace mpc::sampler
{
	class Pad
		: public moduru::observer::Observable
	{

	private:
		static std::vector<int> originalPadNotes;
		
	public:
		static std::vector<int>& getPadNotes(mpc::Mpc& mpc);
		

	private:
		mpc::Mpc& mpc;
		int note = 0;
		int index = 0;

	public:
		void setNote(int i);
		int getNote();
		int getNumber();

	public:
		Pad(mpc::Mpc& mpc, int index);

	};
}
