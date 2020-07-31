#pragma once

namespace mpc { class Mpc; }

namespace mpc::hardware {

	class Slider
	{

	private:
		mpc::Mpc& mpc;
		int value = 0;

	public:
		Slider(mpc::Mpc& mpc);
		void setValue(int i);
		int getValue();

	};
}
