#include "AudioMidiConfig.hpp"

#include <Paths.hpp>
#include <lang/StrUtil.hpp>

#include <file/ByteUtil.hpp>
#include <file/File.hpp>

using namespace mpc::nvram;
using namespace std;

void AudioMidiConfig::loadIO(vector<string>* ioNames) {
	moduru::file::File configFile(mpc::Paths::resPath() + "config.vmp", nullptr);
	if (!configFile.exists() || configFile.getLength() != 514) return;
	configFile.openRead();
	string name;
	configFile.seek(64);
	for (int j = 0; j < 7; j++) {
		name = "";
		for (int i = 0; i < 64; i++) {
			name.push_back(configFile.readByte());
		}
		ioNames->push_back(moduru::lang::StrUtil::trim(name));
	}
	configFile.close();
}

void AudioMidiConfig::load(string& serverName, int& bufferSize) {
	moduru::file::File configFile(mpc::Paths::resPath() + "config.vmp", nullptr);
	if (!configFile.exists() || configFile.getLength() != 514) return;
	configFile.openRead();
	for (int i = 0; i < 64; i++) {
		serverName.push_back(configFile.readByte());
	}
	configFile.seek(512);
	bufferSize = (int)(configFile.readShort());
	configFile.close();
	moduru::lang::StrUtil::trim(serverName);
}

void AudioMidiConfig::save(const vector<string>& names, const int& bufferSize) {
	moduru::file::File configFile(mpc::Paths::resPath() + "config.vmp", nullptr);
	if (configFile.exists()) configFile.del();
	configFile.create();

	vector<char> buf;
	string name;
	for (int j = 0; j < 8; j++) {
		name = names.at(j);
		if (name.length() > 64) name = name.substr(0, 64);

		for (char c : moduru::lang::StrUtil::padRight(name, " ", 64))
			buf.push_back(c);
	}
	short bufShort = (short)(bufferSize);
	auto bufShortBytes = moduru::file::ByteUtil::short2bytes(bufShort);
	buf.push_back(bufShortBytes[1]);
	buf.push_back(bufShortBytes[0]);

	configFile.setData(&buf);
	configFile.close();
}
