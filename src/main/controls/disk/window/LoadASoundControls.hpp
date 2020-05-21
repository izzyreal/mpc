#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

namespace mpc::controls::disk::window {

	class LoadASoundControls
		: public mpc::controls::disk::AbstractDiskControls
	{

	private:
		void keepSound();

	public:
		void turnWheel(int i) override;
		void function(int i) override;
	};
}
