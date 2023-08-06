#include <file/aps/ApsSoundNames.hpp>

#include <file/aps/ApsParser.hpp>
#include <sampler/Sampler.hpp>

#include <StrUtil.hpp>

#include <Util.hpp>

using namespace mpc::file::aps;

ApsSoundNames::ApsSoundNames(const std::vector<char>& loadBytes)
{
    int sound = 0;
    
    while (sound < loadBytes.size() / ApsParser::SOUND_NAME_LENGTH)
    {
        auto nameBytes = Util::vecCopyOfRange(loadBytes, sound * ApsParser::SOUND_NAME_LENGTH, (sound * ApsParser::SOUND_NAME_LENGTH) + NAME_STRING_LENGTH);

        std::string nameStr;
        for (char c : nameBytes)
        {
            if (c == 0x00) break;
            nameStr.push_back(c);
        }
        
        names.push_back(nameStr);
        sound++;
    }
}

ApsSoundNames::ApsSoundNames(mpc::sampler::Sampler* sampler)
{
    saveBytes = std::vector<char>(sampler->getSoundCount() * ApsParser::SOUND_NAME_LENGTH);
    for (int i = 0; i < sampler->getSoundCount(); i++) {
        int offset = i * ApsParser::SOUND_NAME_LENGTH;
        for (int j = 0; j < NAME_STRING_LENGTH; j++)
        saveBytes[offset + j] = StrUtil::padRight(sampler->getSound(i)->getName(), " ", NAME_STRING_LENGTH)[j];
        
        saveBytes[offset + NAME_STRING_LENGTH] = ApsParser::NAME_TERMINATOR;
    }
}

const int ApsSoundNames::NAME_STRING_LENGTH;

std::vector<std::string> ApsSoundNames::get()
{
    return names;
}

std::vector<char> ApsSoundNames::getBytes()
{
    return saveBytes;
}
