#pragma once

namespace mpc::sequencer
{
	class Step
	{

	private:
		int sequence = 0;
		int repeatCount = 1;

	public:
		void setSequence(int i);
		int getSequence();
		void setRepeats(int i);
		int getRepeats();
	};
}
