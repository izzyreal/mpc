#pragma once
#include <disk/device/Device.hpp>

#include <memory>

/*
* A StdDevice, as opposed to a RawDevice (see older commits), is a disk device which root is bound to a directory on the user's
* drive where some APS, SND, WAV, ALL, etc. files are that the user wants to access from within VMPC2000XL.
* 
* Maybe later I'll add RawDevice, an implementation that has only worked in the abandoned Java rendition of VMPC2000XL.
*/

namespace moduru::file
{
		class Directory;
}

namespace mpc::disk::device
{
	class StdDevice
		: public virtual Device
	{

	private:
		bool valid = false;
		std::shared_ptr<moduru::file::Directory> root;

	public:
		nonstd::any getRoot() override;
		virtual bool isValid();
		void close() override;
		void flush() override;
		nonstd::any getFileSystem() override;
		std::string getAbsolutePath() override;
		int getSize() override;
		std::string getVolumeName() override;

		StdDevice(std::string rootPath);

	};
}
