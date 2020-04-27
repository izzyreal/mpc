#include <nvram/KnobPositions.hpp>

#include <StartUp.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

#include <iostream>
#include <fstream>
#include <vector>


using namespace mpc::nvram;

using namespace moduru::file;

using namespace std;

KnobPositions::KnobPositions()
{
	auto file = make_shared<File>(mpc::StartUp::resPath + "knobpositions.vmp", nullptr);

	if (!file->exists()) {

		file->create();

		auto stream = FileUtil::ofstreamw(file->getPath(), ios::binary | ios::out);

		auto bytes = vector<char>(3);
		bytes[0] = static_cast<int8_t>(recordLevel);
		bytes[1] = static_cast<int8_t>(masterLevel);
		bytes[2] = static_cast<int8_t>(slider);

		stream.write(&bytes[0], bytes.size());
		stream.close();
	}
	else {
		auto stream = FileUtil::ifstreamw(file->getPath(), ios::binary | ios::in);
		auto bytes = vector<char>(3);
		stream.read(&bytes[0], bytes.size());
		stream.close();
		recordLevel = bytes[0];
		masterLevel = bytes[1];
		slider = bytes[2];
	}
}
