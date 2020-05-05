#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

#include <disk/ApsSaver.hpp>

#include <memory>

namespace mpc::controls::disk::window
{

	class SaveApsFileControls
		: public mpc::controls::disk::AbstractDiskControls
	{

	private:
		std::unique_ptr<mpc::disk::ApsSaver> apsSaver;

	public:
		typedef mpc::controls::disk::AbstractDiskControls super;
		void turnWheel(int i) override;
		void function(int i) override;

		SaveApsFileControls();

	};
}
