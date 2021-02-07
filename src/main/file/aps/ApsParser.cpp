#include <file/aps/ApsParser.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <VecUtil.hpp>

using namespace moduru;
using namespace mpc::file::aps;
using namespace mpc::disk;
using namespace mpc::sampler;
using namespace std;

ApsParser::ApsParser(mpc::Mpc& mpc, weak_ptr<MpcFile> file)
{
	auto loadBytes = file.lock()->getBytes();
    
    if (loadBytes.size() == 0)
    {
        throw invalid_argument(file.lock()->getName() + " is an empty file");
    }
    
	header = make_unique<ApsHeader>(VecUtil::CopyOfRange(&loadBytes, HEADER_OFFSET, HEADER_OFFSET + HEADER_LENGTH));
    
	auto const soundNamesEnd = HEADER_LENGTH + (header->getSoundAmount() * SOUND_NAME_LENGTH);
	soundNames = make_unique<ApsSoundNames>(VecUtil::CopyOfRange(&loadBytes, HEADER_OFFSET + HEADER_LENGTH, soundNamesEnd));
	programCount = (loadBytes.size() - 1689 - (soundNames->get().size() * 17)) / PROGRAM_LENGTH;
	auto const nameEnd = soundNamesEnd + PAD_LENGTH1 + APS_NAME_LENGTH;
	auto const nameOffset = soundNamesEnd + PAD_LENGTH1;
	apsName = make_unique<ApsName>(VecUtil::CopyOfRange(&loadBytes, nameOffset, nameEnd));
	auto const parametersEnd = nameEnd + PARAMETERS_LENGTH;
	globalParameters = make_unique<ApsGlobalParameters>(mpc, VecUtil::CopyOfRange(&loadBytes, nameEnd, parametersEnd));
	auto const maTableEnd = parametersEnd + TABLE_LENGTH;
	maTable = make_unique<ApsAssignTable>(VecUtil::CopyOfRange(&loadBytes, parametersEnd, maTableEnd));
	int const drum1MixerOffset = maTableEnd + PAD_LENGTH2;

	for (int i = 0; i < 4; i++) {
		int offset = drum1MixerOffset + (i * (MIXER_LENGTH + DRUM_CONFIG_LENGTH + DRUM_PAD_LENGTH));
		drumMixers[i] = make_unique<ApsMixer>(VecUtil::CopyOfRange(&loadBytes, offset, offset + MIXER_LENGTH));
		drumConfigurations[i] = make_unique<ApsDrumConfiguration>(VecUtil::CopyOfRange(&loadBytes, offset + MIXER_LENGTH, offset + MIXER_LENGTH + DRUM_CONFIG_LENGTH));
	}
	
	int const firstProgramOffset = drum1MixerOffset + ((MIXER_LENGTH + DRUM_CONFIG_LENGTH) * 4) + PAD_LENGTH3 - 6;
	
	for (int i = 0; i < programCount; i++)
	{
		int offset = firstProgramOffset + (i * (PROGRAM_LENGTH + PROGRAM_PAD_LENGTH));
		programs.push_back(make_unique<ApsProgram>(VecUtil::CopyOfRange(&loadBytes, offset, offset + PROGRAM_LENGTH)));
	}
}

ApsParser::ApsParser(mpc::Mpc& mpc, string apsNameString)
{
	auto sampler = mpc.getSampler().lock();
	vector<vector<char>> chunks;
	programCount = sampler->getProgramCount();
	int const soundCount = sampler->getSoundCount();
	
	auto header = ApsHeader(soundCount);
	chunks.push_back(header.getBytes());

	auto soundNames = ApsSoundNames(sampler.get());
	chunks.push_back(soundNames.getBytes());
	
	chunks.push_back(vector<char>{ 24, 0 });
	
	auto apsName = ApsName(apsNameString);
	chunks.push_back(apsName.getBytes());
	
	auto parameters = ApsGlobalParameters(mpc);
	chunks.push_back(parameters.getBytes());

	auto masterTable = ApsAssignTable(*sampler->getMasterPadAssign());
	chunks.push_back(masterTable.getBytes());
	chunks.push_back(vector<char>{ 4, 0, (char) 136, 1, 64, 0, 6 });

	for (int i = 0; i < 4; i++)
	{
		auto mixer = ApsMixer(sampler->getDrumStereoMixerChannels(i), sampler->getDrumIndivFxMixerChannels(i));
		auto drumConfig = ApsDrumConfiguration(sampler->getDrumBusProgramNumber(i + 1), sampler->getDrum(i)->receivesPgmChange(), sampler->getDrum(i)->receivesMidiVolume());
	
		chunks.push_back(mixer.getBytes());
		
		if (i < 3) {
			chunks.push_back(drumConfig.getBytes());
		}
		else {
			auto ba = drumConfig.getBytes();
			auto ba1 = vector<char>(7);
			
			for (int j = 0; j < 7; j++)
				ba1[j] = ba[j];

			chunks.push_back(ba1);
		}
	}

    chunks.push_back(vector<char>{ 1, 127 });
	
	for (int i = 0; i < 24; i++)
	{
		auto p = sampler->getProgram(i).lock();
		
		if (!p)
			continue;

		auto program = ApsProgram(dynamic_cast<mpc::sampler::Program*>(p.get()), i);
		chunks.push_back(program.getBytes());
	}
	
	chunks.push_back(vector<char>{ (char)255, (char)255 });
	auto totalSize = 0;
	
	for (auto& ba : chunks)
		totalSize += ba.size();
	
	saveBytes = vector<char>(totalSize);
	auto counter = 0;
	
	for (auto& ba : chunks)
	{
		for (auto& b : ba)
			saveBytes[counter++] = b;
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

vector<string> ApsParser::getSoundNames()
{
    return soundNames->get();
}

bool ApsParser::isHeaderValid()
{
    return header->isValid();
}

string ApsParser::getApsName()
{
    return apsName->get();
}

vector<ApsProgram*> ApsParser::getPrograms()
{
	vector<ApsProgram*> res;
	for (auto& ap : programs)
		res.push_back(ap.get());
	return res;
}

vector<ApsMixer*> ApsParser::getDrumMixers()
{
	vector<ApsMixer*> res;
	for (auto& am : drumMixers)
		res.push_back(am.get());
	return res;
}

ApsDrumConfiguration* ApsParser::getDrumConfiguration(int i)
{
    return drumConfigurations[i].get();
}

ApsGlobalParameters* ApsParser::getGlobalParameters()
{
    return globalParameters.get();
}

vector<char> ApsParser::getBytes()
{
    return saveBytes;
}

ApsParser::~ApsParser() {
}
