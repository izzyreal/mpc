#include "file/all/AllSong.hpp"

#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"

#include "Util.hpp"

using namespace mpc::file::all;

Song::Song(const std::vector<char> &loadBytes)
{
    auto nameBytes = Util::vecCopyOfRange(loadBytes, NAME_OFFSET, NAME_OFFSET + AllParser::NAME_LENGTH);

    for (char &c : nameBytes)
    {
        if (c == 0x00)
        {
            break;
        }
        name.push_back(c);
    }

    size_t end = name.find_last_not_of(' ');
    name = (end == std::string::npos) ? "" : name.substr(0, end + 1);

    auto stepsBytes = Util::vecCopyOfRange(loadBytes, FIRST_STEP_OFFSET, FIRST_STEP_OFFSET + STEPS_LENGTH);

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

    loopFirstStepIndex = loadBytes[LOOP_FIRST_STEP_INDEX_OFFSET];
    loopLastStepIndex = loadBytes[LOOP_LAST_STEP_INDEX_OFFSET];
    loopEnabled = loadBytes[LOOP_ENABLED_OFFSET] == 1;

    // This is what it's supposed to do, but there may be users out there
    // with badly persisted files due to older versions of VMPC2000XL:

    // isUsed = loadBytes[IS_USED_OFFSET];

    // So for now we rely on the following heuristic, as this will
    // correctly parse real MPC2000XL ALL songs, as well as the ones written
    // with older versions of VMPC2000XL, unless someone decides to name
    // their song "(Unused)":
    isUsed = name != "(Unused)";
}

Song::Song(mpc::sequencer::Song *mpcSong)
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

    saveBytes[STEPS_TERMINATOR_OFFSET] = static_cast<char>(0xFF);
    saveBytes[STEPS_TERMINATOR_OFFSET + 1] = static_cast<char>(0xFF);

    saveBytes[IS_USED_OFFSET] = mpcSong->isUsed() ? 1 : 0;
    saveBytes[LOOP_FIRST_STEP_INDEX_OFFSET] = mpcSong->getFirstStep();
    saveBytes[LOOP_LAST_STEP_INDEX_OFFSET] = mpcSong->getLastStep();
    saveBytes[LOOP_ENABLED_OFFSET] = mpcSong->isLoopEnabled() ? 1 : 0;

    for (int i = LOOP_ENABLED_OFFSET + 1; i < LENGTH; i++)
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

int Song::getLoopFirstStepIndex()
{
    return loopFirstStepIndex;
}

int Song::getLoopLastStepIndex()
{
    return loopLastStepIndex;
}

bool Song::isLoopEnabled()
{
    return loopEnabled;
}

std::vector<char> &Song::getBytes()
{
    return saveBytes;
}
