#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc::controls::disk::window {

	class LoadASequenceFromAllControls
		: public mpc::controls::disk::AbstractDiskControls
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

	};
}
