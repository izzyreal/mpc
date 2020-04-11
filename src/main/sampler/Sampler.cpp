#include <sampler/Sampler.hpp>
#include <lang/StrUtil.hpp>
#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <disk/SoundLoader.hpp>
#include <StartUp.hpp>
#include <ui/Uis.hpp>
#include <ui/UserDefaults.hpp>
#include <lcdgui/Background.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequence.hpp>

#include <synth/SynthChannel.hpp>

#include <mpc/MpcBasicSoundPlayerChannel.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <file/File.hpp>

#include <thirdp/libsamplerate/samplerate.h>

#include <audio/server/ExternalAudioServer.hpp>

using namespace mpc::sampler;
using namespace std;

Sampler::Sampler(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	soundSortingType = 0;
	abcd = vector<string>{ "A", "B", "C", "D" };
	sortNames = vector<string>{ "MEMORY", "NAME", "SIZE" };
	auto ud = mpc::StartUp::getUserDefaults().lock();
	initMasterPadAssign = ud->getPadNotes();

	programs = vector<shared_ptr<Program>>(24);
}

void Sampler::setInputLevel(int i) {
	if (i < 0 || i > 100) return;
	inputLevel = i;
}

int Sampler::getInputLevel() {
	return inputLevel;
}

vector<weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> Sampler::getDrumStereoMixerChannels(int i)
{
	return mpc->getDrums()[i]->getStereoMixerChannels();
}

vector<weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> Sampler::getDrumIndivFxMixerChannels(int i)
{
	return mpc->getDrums()[i]->getIndivFxMixerChannels();
}

vector<int>* Sampler::getInitMasterPadAssign()
{
	return &initMasterPadAssign;
}

vector<int>* Sampler::getMasterPadAssign()
{
	return &masterPadAssign;
}

void Sampler::setMasterPadAssign(vector<int> v) {
	masterPadAssign = v;
}

vector<int>* Sampler::getAutoChromaticAssign()
{
	return &autoChromaticAssign;
}

void Sampler::init()
{
	auto program = addProgram().lock();;
	program->setName("NewPgm-A");
	for (int i = 0; i < 4; i++) {
		for (auto j = 0; j < 16; j++) {
			string result = "";
			result.append(abcd[i]);
			result.append(moduru::lang::StrUtil::padLeft(to_string(j + 1), "0", 2));
			padNames.push_back(result);
		}
	}

	auto f = make_shared<moduru::file::File>(mpc::StartUp::resPath + "click.wav", nullptr);
	clickSound = make_shared<Sound>();
	mpc::disk::SoundLoader::getSampleDataFromWav(f, clickSound->getSampleData());
	clickSound->setMono(true);
	clickSound->setLevel(100);
	masterPadAssign = initMasterPadAssign;
	autoChromaticAssign = vector<int>(64);
	for (int i = 0; i < 64; i++)
		autoChromaticAssign[i] = i;
}

void Sampler::playMetronome(mpc::sequencer::NoteEvent* event, int framePos)
{
	auto swGui = mpc->getUis().lock()->getSequencerWindowGui();
	auto soundNumber = -2;
	if (swGui->getMetronomeSound() != 0) {
		auto program = mpc->getDrum(swGui->getMetronomeSound() - 1)->getProgram();
		auto accent = event->getVelocity() == swGui->getAccentVelo();
		soundNumber = programs[program]->getNoteParameters(accent ? swGui->getAccentNote() : swGui->getNormalNote())->getSndNumber();
	}
	mpc->getBasicPlayer()->mpcNoteOn(soundNumber, event->getVelocity(), framePos);
}

void Sampler::playPreviewSample(int start, int end, int loopTo, int overlapMode)
{
	if (sounds.size() == 0) {
		return;
	}

	auto previewSound = sounds[sounds.size() - 1];
	auto oldStart = previewSound->getStart();
	auto oldEnd = previewSound->getEnd();
	auto oldLoopTo = previewSound->getLoopTo();
	previewSound->setStart(start);
	previewSound->setEnd(end);
	previewSound->setLoopTo(loopTo);
	mpc->getBasicPlayer()->noteOn(-3, 127);
	previewSound->setStart(oldStart);
	previewSound->setEnd(oldEnd);
	previewSound->setLoopTo(oldLoopTo);
}

weak_ptr<ctoot::mpc::MpcProgram> Sampler::getProgram(int programNumber)
{
	return programs[programNumber];
}

int Sampler::getProgramCount()
{
	int res = 0;
	for (auto& p : programs)
		if (p) res++;
	return res;
}

weak_ptr<Program> Sampler::addProgram(int i) {
	if (programs[i]) return weak_ptr<Program>();
	programs[i] = make_shared<Program>(mpc, this);
	return programs[i];
}

weak_ptr<Program> Sampler::addProgram()
{
	for (auto& p : programs) {
		if (!p) {
			p = make_shared<Program>(mpc, this);
			return p;
		}
	}
	return weak_ptr<Program>();
}

void Sampler::deleteProgram(weak_ptr<Program> program) {
	auto lProgram = program.lock();
	for (auto&& p : programs) {
		if (p == lProgram) {
			p.reset();
			break;
		}
	}
}

vector<weak_ptr<Sound>> Sampler::getSounds()
{
	auto res = vector<weak_ptr<Sound>>();
	for (auto& s : sounds)
		res.push_back(s);
	return res;
}

weak_ptr<Sound> Sampler::addSound() {
	auto res = make_shared<Sound>(44100, sounds.size());
	sounds.push_back(res);
	return res;
}

weak_ptr<Sound> Sampler::addSound(int sampleRate) {
	auto res = make_shared<Sound>(sampleRate, sounds.size());
	sounds.push_back(res);
	return res;
}

int Sampler::getSoundCount()
{
	return sounds.size();
}

string Sampler::getSoundName(int i)
{
	return sounds[i]->getName();
}

string Sampler::getPadName(int i)
{
	return padNames[i];
}

vector<weak_ptr<Program>> Sampler::getPrograms()
{
	auto res = vector<weak_ptr<Program>>();
	for (auto& p : programs)
		res.push_back(p);
	return res;
}

void Sampler::replaceProgram(weak_ptr<Program> p, int index) {
	auto sourceProgram = p.lock();
	auto destProgram = programs[index];
	destProgram.swap(sourceProgram);
	deleteProgram(sourceProgram);
}

void Sampler::deleteAllPrograms(bool init) {
	for (auto& p : programs)
		p.reset();
	if (!init) return;
	addProgram().lock()->setName("NewPgm-A");
	checkProgramReferences();
}

void Sampler::checkProgramReferences() {
	auto lSequencer = mpc->getSequencer().lock();
	auto t = lSequencer->getActiveSequence().lock()->getTrack(lSequencer->getActiveTrackIndex()).lock();
	auto bus = t->getBusNumber();
	for (int i = 0; i < 4; i++) {
		auto pgm = getDrumBusProgramNumber(bus);
		if (!programs[pgm]) {
			// check what real MPC does in this condition
			for (int i = 0; i < 24; i++) {
				if (programs[i]) {
					setDrumBusProgramNumber(bus, i);
				}
			}
		}
	}
}

vector<float>* Sampler::getClickSample()
{
	return &clickSample;
}

weak_ptr<ctoot::mpc::MpcSound> Sampler::getSound(int sampleNumber)
{
	if (sampleNumber == -1)
		return weak_ptr<Sound>();

	if (sampleNumber >= sounds.size())
		return weak_ptr<Sound>();

	return sounds[sampleNumber];
}

weak_ptr<ctoot::mpc::MpcSound> Sampler::getPreviewSound()
{
	if (sounds.size() == 0) return weak_ptr<Sound>();
	return sounds[sounds.size() - 1];
}

void Sampler::setLoopEnabled(int sampleIndex, bool enabled)
{
	sounds[sampleIndex]->setLoopEnabled(enabled);

	if (!enabled) return;

	for (auto& p : programs) {
		if (!p) continue;
		for (int i = 0; i < 64; i++) {
			if (p->getNoteParameters(i + 35)->getSndNumber() == sampleIndex) {
				dynamic_cast<mpc::sampler::NoteParameters*>(p->getNoteParameters(i + 35))->setVoiceOverlap(2);
			}
		}
	}
}

void Sampler::trimSample(int sampleNumber, int start, int end)
{
	auto s = sounds[sampleNumber];
	trimSample(s, start, end);
}

void Sampler::trimSample(weak_ptr<Sound> sound, int start, int end) {
	auto s = sound.lock();
	auto data = s->getSampleData();
	auto lf = s->getLastFrameIndex();

	if (!s->isMono()) {
		int startRight = start + lf;
		int endRight = end + lf;
		data->erase(data->begin() + endRight, data->end());
		data->erase(data->begin() + lf, data->begin() + startRight);
		data->erase(data->begin() + end, data->begin() + lf);
		data->erase(data->begin(), data->begin() + start);
	}
	else {
		data->erase(data->begin() + end, data->end());
		data->erase(data->begin(), data->begin() + start);
	}
	s->setStart(0);
	s->setEnd(s->getLastFrameIndex() + 1);
	s->setLoopTo(s->getLastFrameIndex() + 1);
}

void Sampler::deleteSection(const unsigned int sampleNumber, const unsigned int start, const unsigned int end) {
	auto s = sounds[sampleNumber];
	auto data = s->getSampleData();
	auto lf = s->getLastFrameIndex();

	if (!s->isMono()) {
		const unsigned int startRight = start + lf;
		const unsigned int endRight = end + lf;
		data->erase(data->begin() + startRight, data->begin() + endRight);
	}
	data->erase(data->begin() + start, data->begin() + end);
}

void Sampler::sort()
{
	auto soundGui = mpc->getUis().lock()->getSoundGui();
	auto currentSound = soundGui->getSoundIndex();
	auto currentSoundMemIndex = sounds[currentSound]->getMemoryIndex();
	soundSortingType++;
	if (soundSortingType > 2) soundSortingType = 0;
	switch (soundSortingType) {
	case 0:
		stable_sort(sounds.begin(), sounds.end(), compareMemoryIndex);
		break;
	case 1:
		stable_sort(sounds.begin(), sounds.end(), compareName);
		break;
	case 2:
		stable_sort(sounds.begin(), sounds.end(), compareSize);
		break;
	}
	for (int i = 0; i < sounds.size(); i++) {
		if (sounds[i]->getMemoryIndex() == currentSoundMemIndex) {
			soundGui->setSoundIndex(i, sounds.size());
			break;
		}
	}
}

void Sampler::deleteSample(int sampleIndex)
{
	sounds.erase(sounds.begin() + sampleIndex);
	for (int i = sampleIndex; i < sounds.size(); i++) {
		sounds[i]->setMemoryIndex(sounds[i]->getMemoryIndex() - 1);
	}

	for (auto& p : programs) {
		if (!p) continue;
		for (auto& n : p->getNotesParameters()) {
			if (n->getSndNumber() == sampleIndex) {
				n->setSoundNumber(-1);
			}
			else if (n->getSndNumber() > sampleIndex) {
				n->setSoundNumber(n->getSndNumber() - 1);
			}
		}
	}
}

void Sampler::deleteAllSamples()
{
	sounds.clear();
	for (auto& p : programs) {
		if (!p) continue;
		for (auto& n : p->getNotesParameters()) {
			n->setSoundNumber(-1);
		}
	}
}

void Sampler::process12Bit(vector<float>* fa)
{
	for (auto j = 0; j < fa->size(); j++) {

		if (fa->at(j) != 0.0f) {
			auto fShort = static_cast< int16_t >(fa->at(j) * 32767.4999999);

			if (fa->at(j) > 0.9999999f)
				fShort = 32767;
			int16_t newShort = fShort;
			newShort &= ~(1 << 0);
			newShort &= ~(1 << 1);
			newShort &= ~(1 << 2);
			newShort &= ~(1 << 3);
			fa->at(j) = static_cast< float >(newShort / 32767.4999999);
		}
		else {
			fa->at(j) = 0;
		}
	}
}

void Sampler::process8Bit(vector<float>* fa)
{
	for (auto j = 0; j < fa->size(); j++) {
		if ((*fa)[j] != 0.0f) {
			float f = (*fa)[j];
			if (f < -1) {
				f = -1;
			}
			else if (f > 1) {
				f = 1;
			}
			unsigned short ushort = (signed short)((f + 1)  * 32767.4999999);
			unsigned char eightBit = ushort >> 8;
			signed sshort = (eightBit - 128) << 8;
			f = (float)(sshort / 32767.49999999);
			if (f < -1) {
				f = -1;
			}
			else if (f > 1) {
				f = 1;
			}
			(*fa)[j] = f;
		}
	}
}

weak_ptr<Sound> Sampler::createZone(weak_ptr<Sound> source, int start, int end, int endMargin)
{
	auto lSource = source.lock();
	auto overlap = (int)(endMargin * (lSource->getSampleRate() / 1000.0));
	if (overlap > end - start) {
		overlap = end - start;
	}
	auto zone = copySound(source);
	auto zoneLength = end - start + overlap;
	trimSample(zone, start, start + zoneLength);
	return zone;
}

void Sampler::stopAllVoices()
{
	if (!mpc->getAudioMidiServices().lock()->getExternalAudioServer()->isRunning()) {
		return;
	}
	mpc->getBasicPlayer()->allSoundOff();
	for (auto m : mpc->getDrums())
		m->allSoundOff();
}

void Sampler::stopAllVoices(int frameOffset) {
	dynamic_cast<ctoot::mpc::MpcSoundPlayerChannel*>(mpc->getDrums()[0])->allSoundOff(frameOffset);
}

void Sampler::finishBasicVoice() {
	mpc->getBasicPlayer()->finishVoice();
}

void Sampler::playX(int playXMode, vector<int>* zone)
{
	int index = mpc->getUis().lock()->getSoundGui()->getSoundIndex();
	auto sound = sounds[index];
	auto start = 0;
	auto end = sound->getSampleData()->size() - 1;

	if (!sound->isMono()) end /= 2;

	auto fullEnd = end;
	if (playXMode == 1) {
		start = (*zone)[0];
		end = (*zone)[1];
	}
	if (playXMode == 2) {
		end = sound->getStart();
	}
	if (playXMode == 3) {
		end = sound->getLoopTo();
	}

	if (playXMode == 4) {
		start = sound->getEnd();
		end = fullEnd;
	}
	int oldStart = sound->getStart();
	int oldEnd = sound->getEnd();
	sound->setStart(start);
	sound->setEnd(end);
	mpc->getBasicPlayer()->noteOn(-4, 127);
	sound->setStart(oldStart);
	sound->setEnd(oldEnd);
}

weak_ptr<ctoot::mpc::MpcSound> Sampler::getPlayXSound() {
	return sounds[mpc->getUis().lock()->getSoundGui()->getSoundIndex()];
}

int Sampler::getFreeSampleSpace()
{
	int freeSpace = 32 * 1024 * 1000;
	for (auto& s : sounds) {
		freeSpace -= s->getSampleData()->size() * 2;
	}
	return freeSpace;
}

int Sampler::getLastInt(string s)
{
	auto offset = s.length();
	for (int i = s.length() - 1; i >= 0; i--) {
		auto c = s.at(i);
		if (isdigit(c)) {
			offset--;
		}
		else {
			if (offset == s.length()) {
				return INT_MIN;
			}
			break;
		}
	}
	return stoi(s.substr(offset));
}

string Sampler::addOrIncreaseNumber(string s)
{
	auto res = addOrIncreaseNumber2(s);
	bool exists = true;
	while (exists) {
		exists = false;
		for (int i = 0; i < getSoundCount(); i++) {
			if (getSoundName(i).compare(res) == 0) {
				exists = true;
				res = addOrIncreaseNumber2(res);
				break;
			}
		}
	}
	return res;
}

string Sampler::addOrIncreaseNumber2(string s) {
	int candidate = getLastInt(s);
	string res = s;
	if (candidate != INT_MIN) {
		auto candidateStr = to_string(candidate);
		int candidateLength = candidateStr.length();

		res = res.substr(0, res.length() - candidateLength);

		candidate++;

		candidateStr = to_string(candidate);
		candidateLength = candidateStr.length();

		if (res.length() + candidateLength > 16)
			res = res.substr(0, 16 - candidateLength);

		res += candidateStr;
	}
	else {
		res = s + to_string(1);
	}
	return res;
}

Pad* Sampler::getLastPad(Program* program)
{
	auto sGui = mpc->getUis().lock()->getSamplerGui();
	auto lastValidPad = sGui->getPad();
	if (lastValidPad == -1) lastValidPad = sGui->getPrevPad();
	return program->getPad(lastValidPad);
}

NoteParameters* Sampler::getLastNp(Program* program)
{
	auto sGui = mpc->getUis().lock()->getSamplerGui();
	auto lastValidNote = sGui->getNote();
	if (lastValidNote == 34) lastValidNote = sGui->getPrevNote();
	return dynamic_cast<mpc::sampler::NoteParameters*>(program->getNoteParameters(lastValidNote));
}

int Sampler::getUnusedSampleAmount()
{
	vector<weak_ptr<Sound>> soundSet;
	for (auto& p : programs) {
		if (!p) continue;
		for (auto& nn : p->getNotesParameters()) {
			if (nn->getSndNumber() != -1) {
				auto candidate = sounds[nn->getSndNumber()];
				bool contains = false;
				for (auto& s : soundSet) {
					if (s.lock() == candidate) {
						contains = true;
						break;
					}
				}
				if (!contains) soundSet.push_back(sounds[nn->getSndNumber()]);
			}
		}
	}
	return (int)(sounds.size() - soundSet.size());
}

void Sampler::purge()
{
	vector<weak_ptr<Sound>> soundSet;
	for (auto& p : programs) {
		for (auto& nn : p->getNotesParameters()) {
			if (nn->getSndNumber() != -1) {
				auto candidate = sounds[nn->getSndNumber()];
				bool contains = false;
				for (auto& s : soundSet) {
					if (s.lock() == candidate) {
						contains = true;
						break;
					}
				}
				if (!contains) soundSet.push_back(sounds[nn->getSndNumber()]);
			}
		}
	}

	vector<weak_ptr<Sound>> toRemove;
	for (auto& s : sounds) {
		bool contains = false;
		for (auto& s2 : soundSet) {
			if (s2.lock() == s) {
				contains = true;
				break;
			}
		}
		if (!contains) toRemove.push_back(s);
	}
	for (auto& s : toRemove) {
		deleteSound(s);
	}
}

void Sampler::deleteSound(weak_ptr<Sound> sound) {
	auto lSound = sound.lock();
	for (int i = 0; i < sounds.size(); i++) {
		if (sounds[i] == lSound) {
			sounds.erase(sounds.begin() + i);
			break;
		}
	}

	stable_sort(sounds.begin(), sounds.end(), compareMemoryIndex);

	for (int i = 0; i < sounds.size(); i++) {
		sounds[i]->setMemoryIndex(i);
	}

	switch (soundSortingType) {
	case 0:
		stable_sort(sounds.begin(), sounds.end(), compareMemoryIndex);
		break;
	case 1:
		stable_sort(sounds.begin(), sounds.end(), compareName);
		break;
	case 2:
		stable_sort(sounds.begin(), sounds.end(), compareSize);
		break;
	}
}

vector<float> Sampler::mergeToStereo(vector<float> fa0, vector<float> fa1)
{
	const int newLengthFrames = fa0.size() > fa1.size() ? fa0.size() : fa1.size();
	vector<float> newSampleData = vector<float>(newLengthFrames * 2);
	for (int i = 0; i < newLengthFrames; i++) {
		if (i < fa0.size()) {
			newSampleData[i] = fa0[i];
		}
		else {
			newSampleData[i] = 0;
		}
		if (i < fa1.size()) {
			newSampleData[i + newLengthFrames] = fa1[i];
		}
		else {
			newSampleData[i + newLengthFrames] = 0;
		}
	}
	return newSampleData;
}

void Sampler::mergeToStereo(vector<float>* sourceLeft, vector<float>* sourceRight, vector<float>* dest) {
	dest->clear();
	for (auto& f : *sourceLeft)
		dest->push_back(f);
	if (sourceLeft->size() < sourceRight->size()) {
		auto diff = sourceRight->size() - sourceLeft->size();
		for (int i = 0; i < diff; i++)
			dest->push_back(0);
	}
	for (auto& f : *sourceRight)
		dest->push_back(f);
	if (sourceRight->size() < sourceLeft->size()) {
		auto diff = sourceLeft->size() - sourceRight->size();
		for (int i = 0; i < diff; i++)
			dest->push_back(0);
	}
}

void Sampler::setDrumBusProgramNumber(int busNumber, int programNumber)
{
	mpc->getDrums()[busNumber - 1]->setProgram(programNumber);
}

int Sampler::getDrumBusProgramNumber(int busNumber)
{
	return mpc->getDrums()[busNumber - 1]->getProgram();
}

ctoot::mpc::MpcSoundPlayerChannel* Sampler::getDrum(int i)
{
	return mpc->getDrum(i);
}

weak_ptr<ctoot::mpc::MpcSound> Sampler::getClickSound()
{
	return clickSound;
}

void Sampler::resample(std::vector<float>* src, int srcRate, std::vector<float>* dest, int destRate) {

	float* srcArray = &(*src)[0];

	SRC_DATA srcData;
	srcData.data_in = srcArray;
	srcData.input_frames = src->size();
	srcData.src_ratio = (double)(destRate) / (double)(srcRate);
	srcData.output_frames = (floor)(src->size() * srcData.src_ratio);

	dest->resize(srcData.output_frames);

	float* destArray = &(*dest)[0];
	srcData.data_out = destArray;

	auto error = src_simple(&srcData, 0, 1);
	if (error != 0) {
		const char* errormsg = src_strerror(error);
		string errorStr(errormsg);
		MLOG("libsamplerate error: " + errorStr);
	}
	for (auto& f : *dest) {
		if (f > 1) {
			f = 1;
		}
		else if (f < -1) {
			f = -1;
		}
	}
}


void Sampler::setSampleBackground()
{
	mpc->getLayeredScreen().lock()->getCurrentBackground()->setName("sample");
	auto components = mpc->getLayeredScreen().lock()->getLayer(0)->getAllLabelsAndFields();
	for (auto& c : components) {
		c.lock()->SetDirty();
	}
	mpc->getLayeredScreen().lock()->getFunctionKeys()->SetDirty();
}

int Sampler::checkExists(string soundName)
{
	for (int i = 0; i < getSoundCount(); i++) {
		if (moduru::lang::StrUtil::eqIgnoreCase(moduru::lang::StrUtil::replaceAll(soundName, ' ', ""), getSoundName(i))) {
			return i;
		}
	}
	return -1;
}

int Sampler::getNextSoundIndex(int j, bool up)
{
	auto inc = up ? 1 : -1;
	if (j + inc < -1 || j + inc > getSoundCount() - 1) return j;
	return j + inc;
}

void Sampler::setSoundGuiPrevSound()
{
	auto soundGui = mpc->getUis().lock()->getSoundGui();
	soundGui->setSoundIndex(getNextSoundIndex(soundGui->getSoundIndex(), false), getSoundCount());
}

void Sampler::setSoundGuiNextSound()
{
	auto soundGui = mpc->getUis().lock()->getSoundGui();
	soundGui->setSoundIndex(getNextSoundIndex(soundGui->getSoundIndex(), true), getSoundCount());
}

weak_ptr<Sound> Sampler::copySound(weak_ptr<Sound> source)
{
	auto sound = source.lock();
	auto newSound = addSound(sound->getSampleRate()).lock();
	newSound->setName(sound->getName());
	newSound->setLoopEnabled(sound->isLoopEnabled());
	auto dest = newSound->getSampleData();
	auto src = sound->getSampleData();
	dest->reserve(src->size());
	copy(src->begin(), src->end(), back_inserter(*dest));
	newSound->setMono(sound->isMono());
	newSound->setEnd(sound->getEnd());
	newSound->setStart(sound->getStart());
	newSound->setLoopTo(sound->getLoopTo());
	return newSound;
}

bool Sampler::compareMemoryIndex(weak_ptr<Sound> a, weak_ptr<Sound> b) {
	return a.lock()->getMemoryIndex() < b.lock()->getMemoryIndex();
}

bool Sampler::compareName(weak_ptr<Sound> a, weak_ptr<Sound> b) {
	return a.lock()->getName() < b.lock()->getName();
}

bool Sampler::compareSize(weak_ptr<Sound> a, weak_ptr<Sound> b) {
	return a.lock()->getLastFrameIndex() < b.lock()->getLastFrameIndex();
}

int Sampler::getUsedProgram(int startIndex, bool up) {
	auto res = startIndex;

	if (up) {
		for (int i = startIndex + 1; i < programs.size(); i++) {
			if (programs[i]) {
				res = i;
				break;
			}
		}
	}
	else {
		for (int i = startIndex - 1; i >= 0; i--) {
			if (programs[i]) {
				res = i;
				break;
			}
		}
	}
	return res;
}

Sampler::~Sampler() {
}
