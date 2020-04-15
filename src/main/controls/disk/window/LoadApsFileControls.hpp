#pragma once
#include <controls/disk/AbstractDiskControls.hpp>

#include <disk/ApsLoader.hpp>

#include <memory>

namespace mpc::controls::disk::window {

	class LoadApsFileControls
		: public mpc::controls::disk::AbstractDiskControls
	{

	private:
		std::unique_ptr<mpc::disk::ApsLoader> apsLoader;

	public:
		typedef mpc::controls::disk::AbstractDiskControls super;
		void function(int i) override;

		LoadApsFileControls(mpc::Mpc* mpc);

	};

}
