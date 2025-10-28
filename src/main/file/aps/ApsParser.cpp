#include "file/aps/ApsParser.hpp"

#include <Mpc.hpp>

#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include <Util.hpp>

using namespace mpc::file::aps;
using namespace mpc::disk;
using namespace mpc::sampler;

ApsParser::ApsParser(const std::vector<char> &loadBytes)
{
    if (loadBytes.empty())
    {
        //        throw invalid_argument(name + " has no data");
        return;
    }

    header = std::make_unique<ApsHeader>(Util::vecCopyOfRange(
        loadBytes, HEADER_OFFSET, HEADER_OFFSET + HEADER_LENGTH));

    auto const soundNamesEnd =
        HEADER_LENGTH + (header->getSoundAmount() * SOUND_NAME_LENGTH);
    soundNames = std::make_unique<ApsSoundNames>(Util::vecCopyOfRange(
        loadBytes, HEADER_OFFSET + HEADER_LENGTH, soundNamesEnd));
    programCount = (loadBytes.size() - 1689 - (soundNames->get().size() * 17)) /
                   PROGRAM_LENGTH;
    auto const nameEnd = soundNamesEnd + PAD_LENGTH1 + APS_NAME_LENGTH;
    auto const nameOffset = soundNamesEnd + PAD_LENGTH1;
    apsName = std::make_unique<ApsName>(
        Util::vecCopyOfRange(loadBytes, nameOffset, nameEnd));
    auto const parametersEnd = nameEnd + PARAMETERS_LENGTH;
    globalParameters = std::make_unique<ApsGlobalParameters>(
        Util::vecCopyOfRange(loadBytes, nameEnd, parametersEnd));
    auto const maTableEnd = parametersEnd + TABLE_LENGTH;
    maTable = std::make_unique<ApsAssignTable>(
        Util::vecCopyOfRange(loadBytes, parametersEnd, maTableEnd));
    int const drum1MixerOffset = maTableEnd + PAD_LENGTH2;

    for (int i = 0; i < 4; i++)
    {
        int offset =
            drum1MixerOffset +
            (i * (MIXER_LENGTH + DRUM_CONFIG_LENGTH + DRUM_PAD_LENGTH));
        drumMixers[i] = std::make_unique<ApsMixer>(
            Util::vecCopyOfRange(loadBytes, offset, offset + MIXER_LENGTH));
        drumConfigurations[i] = std::make_unique<ApsDrumConfiguration>(
            Util::vecCopyOfRange(loadBytes, offset + MIXER_LENGTH,
                                 offset + MIXER_LENGTH + DRUM_CONFIG_LENGTH));
    }

    int const firstProgramOffset = drum1MixerOffset +
                                   ((MIXER_LENGTH + DRUM_CONFIG_LENGTH) * 4) +
                                   PAD_LENGTH3 - 6;

    for (int i = 0; i < programCount; i++)
    {
        int offset =
            firstProgramOffset + (i * (PROGRAM_LENGTH + PROGRAM_PAD_LENGTH));
        programs.push_back(std::make_unique<ApsProgram>(
            Util::vecCopyOfRange(loadBytes, offset, offset + PROGRAM_LENGTH)));
    }
}

ApsParser::ApsParser(mpc::Mpc &mpc, std::string apsNameString)
{
    auto sampler = mpc.getSampler();
    std::vector<std::vector<char>> chunks;
    programCount = sampler->getProgramCount();
    int const soundCount = sampler->getSoundCount();

    chunks.push_back(ApsHeader(soundCount).getBytes());

    chunks.push_back(ApsSoundNames(sampler.get()).getBytes());

    chunks.push_back(std::vector<char>{24, 0});

    chunks.push_back(ApsName(apsNameString).getBytes());

    auto parameters = ApsGlobalParameters(mpc);
    chunks.push_back(parameters.getBytes());

    auto masterTable = ApsAssignTable(*sampler->getMasterPadAssign());
    chunks.push_back(masterTable.getBytes());
    chunks.push_back(std::vector<char>{4, 0, (char)136, 1, 64, 0, 6});

    for (int i = 0; i < Mpc2000XlSpecs::DRUM_BUS_COUNT; i++)
    {
        auto drumBus = mpc.getSequencer()->getDrumBus(i);
        
        ApsMixer mixer(drumBus->getStereoMixerChannels(),
                       drumBus->getIndivFxMixerChannels());

        ApsDrumConfiguration drumConfig(drumBus->getProgram(),
                                        drumBus->receivesPgmChange(),
                                        drumBus->receivesMidiVolume());

        chunks.push_back(mixer.getBytes());

        if (i < 3)
        {
            chunks.push_back(drumConfig.getBytes());
        }
        else
        {
            auto ba = drumConfig.getBytes();
            auto ba1 = std::vector<char>(7);

            for (int j = 0; j < 7; j++)
            {
                ba1[j] = ba[j];
            }

            chunks.push_back(ba1);
        }
    }

    chunks.push_back({1, 127});

    for (int i = 0; i < 24; i++)
    {
        auto p = sampler->getProgram(i);

        if (!p)
        {
            continue;
        }

        auto program =
            ApsProgram(dynamic_cast<mpc::sampler::Program *>(p.get()), i);
        chunks.push_back(program.getBytes());
    }

    chunks.push_back(std::vector<char>{(char)255, (char)255});
    auto totalSize = 0;

    for (auto &ba : chunks)
    {
        totalSize += ba.size();
    }

    saveBytes = std::vector<char>(totalSize);
    auto counter = 0;

    for (auto &ba : chunks)
    {
        for (auto &b : ba)
        {
            saveBytes[counter++] = b;
        }
    }
}

const int ApsParser::HEADER_OFFSET;
const int ApsParser::HEADER_LENGTH;
const int ApsParser::SOUND_NAME_LENGTH;
const int ApsParser::PAD_LENGTH1;
const int ApsParser::APS_NAME_LENGTH;
const int ApsParser::PARAMETERS_LENGTH;
const int ApsParser::TABLE_LENGTH;
const int ApsParser::PAD_LENGTH2;
const int ApsParser::MIXER_LENGTH;
const int ApsParser::DRUM_CONFIG_LENGTH;
const int ApsParser::DRUM_PAD_LENGTH;
const int ApsParser::PAD_LENGTH3;
const int ApsParser::PROGRAM_LENGTH;
const int ApsParser::PROGRAM_PAD_LENGTH;
const int ApsParser::LAST_PROGRAM_PAD_LENGTH;
const char ApsParser::NAME_TERMINATOR;

std::vector<std::string> ApsParser::getSoundNames()
{
    return soundNames->get();
}

bool ApsParser::isHeaderValid()
{
    return header->isValid();
}

std::string ApsParser::getApsName()
{
    return apsName->get();
}

std::vector<ApsProgram *> ApsParser::getPrograms()
{
    std::vector<ApsProgram *> res;
    for (auto &ap : programs)
    {
        res.push_back(ap.get());
    }
    return res;
}

std::vector<ApsMixer *> ApsParser::getDrumMixers()
{
    std::vector<ApsMixer *> res;
    for (auto &am : drumMixers)
    {
        res.push_back(am.get());
    }
    return res;
}

ApsDrumConfiguration *ApsParser::getDrumConfiguration(int i)
{
    return drumConfigurations[i].get();
}

ApsGlobalParameters *ApsParser::getGlobalParameters()
{
    return globalParameters.get();
}

std::vector<char> ApsParser::getBytes()
{
    return saveBytes;
}
