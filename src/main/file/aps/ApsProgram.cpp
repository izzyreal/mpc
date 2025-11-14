#include "file/aps/ApsProgram.hpp"

#include "file/aps/ApsAssignTable.hpp"
#include "file/aps/ApsNoteParameters.hpp"
#include "file/aps/ApsSlider.hpp"
#include "file/pgmwriter/Pads.hpp"
#include "engine/StereoMixer.hpp"
#include "sampler/Program.hpp"

#include "StrUtil.hpp"
#include "Util.hpp"

using namespace mpc::file::aps;
using namespace mpc::engine;

ApsProgram::ApsProgram(const std::vector<char> &loadBytes)
{
    index = loadBytes[INDEX_OFFSET];
    const auto nameBytes =
        Util::vecCopyOfRange(loadBytes, NAME_OFFSET, NAME_OFFSET + NAME_LENGTH);

    name = "";

    for (const char c : nameBytes)
    {
        if (c == 0x00)
        {
            break;
        }

        name.push_back(c);
    }

    name = StrUtil::trim(name);

    slider = new ApsSlider(Util::vecCopyOfRange(loadBytes, SLIDER_OFFSET,
                                                SLIDER_OFFSET + SLIDER_LENGTH));

    for (int i = 0; i < 64; i++)
    {
        const int offset = NOTE_PARAMETERS_OFFSET + i * NOTE_PARAMETERS_LENGTH;
        noteParameters[i] = new ApsNoteParameters(Util::vecCopyOfRange(
            loadBytes, offset, offset + NOTE_PARAMETERS_LENGTH));
    }

    mixer =
        new ApsMixer(Util::vecCopyOfRange(loadBytes, MIXER_OFFSET, MIXER_END));
    assignTable = new ApsAssignTable(
        Util::vecCopyOfRange(loadBytes, ASSIGN_TABLE_OFFSET,
                             ASSIGN_TABLE_OFFSET + ASSIGN_TABLE_LENGTH));
}

ApsProgram::ApsProgram(sampler::Program *program, int index)
{
    std::vector<std::vector<char>> byteList;
    this->index = index;
    byteList.push_back({static_cast<char>(index)});
    byteList.push_back(UNKNOWN);
    auto programName = StrUtil::padRight(program->getName(), " ", 16);

    for (char c : programName)
    {
        byteList.push_back({c});
    }

    byteList.push_back({0}); // Name terminator

    ApsSlider apsSlider(program->getSlider());
    byteList.push_back(apsSlider.getBytes());
    byteList.push_back({35, 64, 0, 26, 0});

    for (int i = 0; i < 64; i++)
    {
        ApsNoteParameters np((program->getNoteParameters(i + 35)));
        byteList.push_back(np.getBytes());
    }

    byteList.push_back({6});

    std::vector<std::shared_ptr<StereoMixer>> stereoMixerChannels(64);
    std::vector<std::shared_ptr<IndivFxMixer>> indivFxMixerChannels(64);

    for (int i = 0; i < 64; i++)
    {
        stereoMixerChannels[i] = program->getStereoMixerChannel(i);
        indivFxMixerChannels[i] = program->getIndivFxMixerChannel(i);
    }

    ApsMixer apsMixer(stereoMixerChannels, indivFxMixerChannels);
    byteList.push_back(apsMixer.getBytes());
    byteList.push_back({0, Mpc2000XlSpecs::PROGRAM_PAD_COUNT, 0});
    auto apsAssignTable =
        std::vector<DrumNoteNumber>(Mpc2000XlSpecs::PROGRAM_PAD_COUNT);

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
    {
        apsAssignTable[i] = program->getNoteFromPad(ProgramPadIndex(i));
    }

    ApsAssignTable table(apsAssignTable);
    byteList.push_back(table.getBytes());
    byteList.push_back(pgmwriter::Pads::getFxBoardSettings());
    auto totalSize = 0;

    for (auto &ba : byteList)
    {
        totalSize += ba.size();
    }

    saveBytes = std::vector<char>(totalSize);
    auto counter = 0;

    for (auto &ba : byteList)
    {
        for (auto b : ba)
        {
            saveBytes[counter++] = b;
        }
    }
}

const int ApsProgram::NAME_OFFSET;
const int ApsProgram::NAME_LENGTH;
const int ApsProgram::NAME_TERMINATOR_LENGTH;
const int ApsProgram::SLIDER_OFFSET;
const int ApsProgram::SLIDER_LENGTH;
const int ApsProgram::PADDING1_LENGTH;
const int ApsProgram::NOTE_PARAMETERS_OFFSET;
const int ApsProgram::NOTE_PARAMETERS_LENGTH;
const int ApsProgram::PADDING2_LENGTH;
const int ApsProgram::MIXER_OFFSET;
const int ApsProgram::MIXER_END;
const int ApsProgram::PADDING3_LENGTH;
const int ApsProgram::ASSIGN_TABLE_OFFSET;
const int ApsProgram::ASSIGN_TABLE_LENGTH;

ApsNoteParameters *ApsProgram::getNoteParameters(const int noteIndex) const
{
    return noteParameters[noteIndex];
}

ApsMixer *ApsProgram::getMixer() const
{
    return mixer;
}

ApsAssignTable *ApsProgram::getAssignTable() const
{
    return assignTable;
}

ApsSlider *ApsProgram::getSlider() const
{
    return slider;
}

std::string ApsProgram::getName()
{
    return name;
}

std::vector<char> ApsProgram::getBytes()
{
    return saveBytes;
}

StereoMixer ApsProgram::getStereoMixerChannel(const int noteIndex) const
{
    return mixer->getStereoMixerChannel(noteIndex);
}

IndivFxMixer ApsProgram::getIndivFxMixerChannel(const int noteIndex) const
{
    return mixer->getIndivFxMixerChannel(noteIndex);
}

ApsProgram::~ApsProgram()
{
    if (slider != nullptr)
    {
        delete slider;
    }
    if (mixer != nullptr)
    {
        delete mixer;
    }
    if (assignTable != nullptr)
    {
        delete assignTable;
    }

    for (const auto &np : noteParameters)
    {
        if (np != nullptr)
        {
            delete np;
        }
    }
}
