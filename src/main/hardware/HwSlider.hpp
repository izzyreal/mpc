#pragma once

namespace mpc::hardware {

	class Slider
	{

	private:
		int value = 0;

	public:
		void setValue(int i);
		int getValue();

	};
}
