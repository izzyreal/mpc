#include <file/aps/ApsSoundNames.hpp>

#include <file/aps/ApsParser.hpp>
#include <sampler/Sampler.hpp>

#include <lang/StrUtil.hpp>

#include <VecUtil.hpp>

using namespace mpc::file::aps;
using namespace moduru::lang;
using namespace moduru;
using namespace std;

ApsSoundNames::ApsSoundNames(vector<char> loadBytes)
{
	int sound = 0;
	while (sound < loadBytes.size() / ApsParser::SOUND_NAME_LENGTH) {
		string nameStr = "";
		auto nameBytes = VecUtil::CopyOfRange(&loadBytes, sound * ApsParser::SOUND_NAME_LENGTH, (sound * ApsParser::SOUND_NAME_LENGTH) + NAME_STRING_LENGTH);
		for (char c : nameBytes) {
			if (c == 0x00) break;
			nameStr.push_back(c);
		}
		names.push_back(nameStr);
		sound++;
	}
}

ApsSoundNames::ApsSoundNames(mpc::sampler::Sampler* sampler)
{
	saveBytes = vector<char>(sampler->getSoundCount() * ApsParser::SOUND_NAME_LENGTH);
	for (int i = 0; i < sampler->getSoundCount(); i++) {
		int offset = i * ApsParser::SOUND_NAME_LENGTH;
		for (int j = 0; j < NAME_STRING_LENGTH; j++)
			saveBytes[offset + j] = StrUtil::padRight(dynamic_pointer_cast<mpc::sampler::Sound>(sampler->getSound(i).lock())->getName(), " ", NAME_STRING_LENGTH)[j];

		saveBytes[offset + NAME_STRING_LENGTH] = ApsParser::NAME_TERMINATOR;
	}
}

const int ApsSoundNames::NAME_STRING_LENGTH;

vector<string> ApsSoundNames::get()
{
    return names;
}

vector<char> ApsSoundNames::getBytes()
{
    return saveBytes;
}
