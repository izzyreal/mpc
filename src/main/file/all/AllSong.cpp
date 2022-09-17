#include <file/all/AllSong.hpp>

#include <sequencer/Song.hpp>
#include <sequencer/Step.hpp>

#include <VecUtil.hpp>

using namespace mpc::file::all;

Song::Song(const std::vector<char>& loadBytes)
{

    auto nameBytes = moduru::VecUtil::CopyOfRange(loadBytes, NAME_OFFSET, NAME_OFFSET + AllParser::NAME_LENGTH);

	for (char& c : nameBytes)
    {
		if (c == 0x00) break;
		name.push_back(c);
	}

    size_t end = name.find_last_not_of(' ');
    name = (end == std::string::npos) ? "" : name.substr(0, end + 1);

    auto stepsBytes = moduru::VecUtil::CopyOfRange(loadBytes, FIRST_STEP_OFFSET, FIRST_STEP_OFFSET + STEPS_LENGTH);

    for (int i = 0; i < STEPS_LENGTH; i += STEP_LENGTH)
    {
        uint8_t seqIndex = stepsBytes[i];
        uint8_t repeatCount = stepsBytes[i + 1];

        if (seqIndex == 0xFF || repeatCount == 0xFF)
        {
            continue;
        }

        steps.emplace_back(seqIndex, repeatCount);
    }

    isUsed = loadBytes[IS_USED_OFFSET];

    /*
     * The AllSong parser was in a very bad shape for most of its lifetime.
     * One of the things it did was incorrectly store unused songs as used.
     * This implies there are users out there who have such corrupted ALL
     * files created with VMPC2000XL. The below serves as a sanitation hack.
     */
    if (isUsed && name == "(Unused)")
    {
        isUsed = false;
    }
}

Song::Song(mpc::sequencer::Song* mpcSong)
{
    auto songName = mpcSong->getName();

    for (int i = 0; i < AllParser::NAME_LENGTH; i++)
    {
        if (i >= songName.length())
        {
            saveBytes[NAME_OFFSET + i] = ' ';
            continue;
        }

        saveBytes[NAME_OFFSET + i] = songName[i];
	}

	for (int i = 0; i < STEP_COUNT; i++)
    {
        if (i >= mpcSong->getStepCount())
        {
            saveBytes[FIRST_STEP_OFFSET + (i * 2)] = static_cast<char>(0xFF);
            saveBytes[FIRST_STEP_OFFSET + (i * 2) + 1] = static_cast<char>(0xFF);
            continue;
        }

        auto step = mpcSong->getStep(i).lock();
        saveBytes[FIRST_STEP_OFFSET + (i * 2)] = static_cast<char>(step->getSequence());
        saveBytes[FIRST_STEP_OFFSET + (i * 2) + 1] = static_cast<char>(step->getRepeats());
    }

    saveBytes[IS_USED_OFFSET] = mpcSong->isUsed() ? 1 : 0;

	for (int i = IS_USED_OFFSET + 1; i < LENGTH; i++)
    {
        saveBytes[i] = 0;
    }
}

std::string Song::getName()
{
    return name;
}

std::vector<std::pair<uint8_t, uint8_t>> Song::getSteps()
{
    return steps;
}

bool Song::getIsUsed()
{
    return isUsed;
}

std::vector<char>& Song::getBytes()
{
    return saveBytes;
}
