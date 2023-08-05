#include <file/aps/ApsProgram.hpp>

#include <file/aps/ApsAssignTable.hpp>
#include <file/aps/ApsNoteParameters.hpp>
#include <file/aps/ApsSlider.hpp>
#include <file/pgmwriter/Pads.hpp>
#include "engine/StereoMixer.hpp"
#include <sampler/Program.hpp>
#include <sampler/Pad.hpp>

#include <StrUtil.hpp>
#include <VecUtil.hpp>

using namespace mpc::file::aps;
using namespace mpc::engine;
using namespace moduru;

ApsProgram::ApsProgram(const std::vector<char>& loadBytes)
{
	index = loadBytes[INDEX_OFFSET];
	auto nameBytes = VecUtil::CopyOfRange(loadBytes, NAME_OFFSET, NAME_OFFSET + NAME_LENGTH);

    name = "";
	
	for (char c : nameBytes)
	{
		if (c == 0x00)
			break;
		
		name.push_back(c);
	}

    name = StrUtil::trim(name);

	slider = new ApsSlider(VecUtil::CopyOfRange(loadBytes, SLIDER_OFFSET, SLIDER_OFFSET + SLIDER_LENGTH));
	
	for (int i = 0; i < 64; i++)
	{
		int offset = NOTE_PARAMETERS_OFFSET + (i * NOTE_PARAMETERS_LENGTH);
		noteParameters[i] = new ApsNoteParameters(VecUtil::CopyOfRange(loadBytes, offset, offset + NOTE_PARAMETERS_LENGTH));
	}

	mixer = new ApsMixer(VecUtil::CopyOfRange(loadBytes, MIXER_OFFSET, MIXER_END));
	assignTable = new ApsAssignTable(VecUtil::CopyOfRange(loadBytes, ASSIGN_TABLE_OFFSET, ASSIGN_TABLE_OFFSET + ASSIGN_TABLE_LENGTH));
}

ApsProgram::ApsProgram(mpc::sampler::Program* program, int index)
{
	std::vector<std::vector<char>> byteList;
	this->index = index;
	byteList.push_back({ (char) index });
	byteList.push_back(UNKNOWN);
	auto programName = StrUtil::padRight(program->getName(), " ", 16);

	for (char c : programName)
		byteList.push_back({ c });
	
	byteList.push_back({ 0 }); // Name terminator

	ApsSlider apsSlider(program->getSlider());
	byteList.push_back(apsSlider.getBytes());
	byteList.push_back({ 35, 64, 0, 26, 0 });

	for (int i = 0; i < 64; i++)
	{
		ApsNoteParameters np(dynamic_cast<mpc::sampler::NoteParameters*>(program->getNoteParameters(i + 35)));
		byteList.push_back(np.getBytes());
	}
	
	byteList.push_back({ 6 });

	std::vector<std::shared_ptr<StereoMixer>> stereoMixerChannels(64);
	std::vector<std::shared_ptr<IndivFxMixer>> indivFxMixerChannels(64);
	
	for (int i = 0; i < 64; i++)
	{
        stereoMixerChannels[i] = program->getStereoMixerChannel(i);
        indivFxMixerChannels[i] = program->getIndivFxMixerChannel(i);
	}

	ApsMixer apsMixer(stereoMixerChannels, indivFxMixerChannels);
	byteList.push_back(apsMixer.getBytes());
	byteList.push_back({ 0, 64, 0 });
	auto apsAssignTable = std::vector<int>(64);
	
	for (int i = 0; i < 64; i++)
		apsAssignTable[i] = program->getNoteFromPad(i);

	ApsAssignTable table(apsAssignTable);
	byteList.push_back(table.getBytes());
	byteList.push_back(mpc::file::pgmwriter::Pads::getFxBoardSettings());
	auto totalSize = 0;

	for (auto& ba : byteList)
		totalSize += ba.size();

	saveBytes = std::vector<char>(totalSize);
	auto counter = 0;

	for (auto& ba : byteList)
	{
		for (auto b : ba)
			saveBytes[counter++] = b;
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

ApsNoteParameters* ApsProgram::getNoteParameters(int noteIndex)
{
    return noteParameters[noteIndex];
}

ApsMixer* ApsProgram::getMixer()
{
    return mixer;
}

ApsAssignTable* ApsProgram::getAssignTable()
{
    return assignTable;
}

ApsSlider* ApsProgram::getSlider()
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

StereoMixer ApsProgram::getStereoMixerChannel(int noteIndex)
{
	return mixer->getStereoMixerChannel(noteIndex);
}

IndivFxMixer ApsProgram::getIndivFxMixerChannel(int noteIndex)
{
	return mixer->getIndivFxMixerChannel(noteIndex);
}

ApsProgram::~ApsProgram()
{
	if (slider != nullptr) delete slider;
	if (mixer != nullptr)delete mixer;
	if (assignTable != nullptr) delete assignTable;
	
	for (auto& np : noteParameters)
		if (np != nullptr) delete np;
}
