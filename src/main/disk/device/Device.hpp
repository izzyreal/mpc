#pragma once

#include <thirdp/any.hpp>

#include <string>

/*
* A Device is a floppy or SCSI disk device in the world of the virtual MPC2000XL.
* See p152 of the manual: https://www.platinumaudiolab.com/free_stuff/manuals/Akai/akai_mpc2000xl_manual.pdf
*/

namespace mpc::disk::device
{

	class Device
	{
	public:
		virtual nonstd::any getRoot() = 0;
		virtual void close() = 0;
		virtual void flush() = 0;
		virtual nonstd::any getFileSystem() = 0;
		virtual std::string getAbsolutePath() = 0;
		virtual int getSize() = 0;
		virtual std::string getVolumeName() = 0;
		virtual ~Device() {};
	};
}
