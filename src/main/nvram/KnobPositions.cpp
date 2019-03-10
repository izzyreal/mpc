#include <nvram/KnobPositions.hpp>

#include <StartUp.hpp>

#include <io/FileOutputStream.hpp>
#include <io/FileInputStream.hpp>

#include <iostream>
#include <fstream>

#include <vector>

using namespace std;
using namespace mpc::nvram;

KnobPositions::KnobPositions()
{
	auto file = make_shared<moduru::file::File>(mpc::StartUp::resPath + "knobpositions.vmp", nullptr);

	if (!file->exists()) {
		file->create();
		auto fos = new moduru::io::FileOutputStream(file.get());
		auto bytes = vector<char>(3);
		bytes[0] = static_cast<int8_t>(recordLevel);
		bytes[1] = static_cast<int8_t>(masterLevel);
		bytes[2] = static_cast<int8_t>(slider);
		fos->write(bytes);
		fos->close();
	}
	else {
		auto fis = new moduru::io::FileInputStream(file);
		auto bytes = vector<char>(3);
		fis->read(&bytes);
		fis->close();
		recordLevel = bytes[0];
		masterLevel = bytes[1];
		slider = bytes[2];
	}
}
