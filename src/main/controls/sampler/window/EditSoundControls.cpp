#include <controls/sampler/window/EditSoundControls.hpp>

#include <Mpc.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sampler/TimeStretch1.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

EditSoundControls::EditSoundControls(mpc::Mpc* mpc) 
	: AbstractSamplerControls(mpc)
{
}

void EditSoundControls::turnWheel(int i)
{
	init();
	auto lLs = ls.lock();
	if (param.compare("edit") == 0) {
		editSoundGui->setEdit(editSoundGui->getEdit() + i);
	}
	else if (param.compare("variable0") == 0 && (editSoundGui->getEdit() == 2 || editSoundGui->getEdit() == 7)) {
		nameGui->setName(lLs->lookupField("variable0").lock()->getText());
		nameGui->setParameterName("newname");
		lLs->openScreen("name");
	}
	else if (param.compare("variable0") == 0 && editSoundGui->getEdit() == 3) {
		editSoundGui->setInsertSndNr(editSoundGui->getInsertSndNr() + i, sampler.lock()->getSoundCount());
	}
	else if (param.compare("variable1") == 0) {
		editSoundGui->setTimeStretchRatio(editSoundGui->getTimeStretchRatio() + i);
	}
	else if (param.compare("variable2") == 0) {
		editSoundGui->setTimeStretchPresetNumber(editSoundGui->getTimeStretchPresetNumber() + i);
	}
	else if (param.compare("variable3") == 0) {
		editSoundGui->setTimeStretchAdjust(editSoundGui->getTimeStretchAdjust() + i);
	}
	else if (param.compare("endmargin") == 0) {
		editSoundGui->setEndMargin(editSoundGui->getEndMargin() + i);
	}
	else if (param.compare("createnewprogram") == 0) {
		editSoundGui->setCreateNewProgram(true);
	}
}

void EditSoundControls::function(int j)
{
	super::function(j);
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	shared_ptr<mpc::sampler::Sound> sound;

	switch (j) {

	case 4:
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
		
		if (editSoundGui->getEdit() == 0) {
			auto newLength = sound->getEnd() - sound->getStart();
			if (sound->getStart() > sound->getLoopTo()) {
				if (sound->getLoopTo() > newLength) {
					sound->setLoopTo(newLength);
				}
				else {
					sound->setLoopTo(0);
				}
			}
			else {
				sound->setLoopTo(sound->getLoopTo() - sound->getStart());
			}
			lSampler->trimSample(soundGui->getSoundIndex(), sound->getStart(), sound->getEnd());
			sound->setStart(0);
			sound->setEnd(newLength);
			sound->setMono(sound->isMono());
		}
		else if (editSoundGui->getEdit() == 1) {
			sound->setLoopTo(sound->getStart());
		}
		else if (editSoundGui->getEdit() == 2) {
			auto newSample = lSampler->addSound().lock();
			newSample->setSampleRate(sound->getSampleRate());
			newSample->setName(editSoundGui->getNewName());
			auto newSampleData = newSample->getSampleData();
			newSampleData->resize(sound->getSampleData()->size());
			for (int i = 0; i < newSampleData->size(); i++) {
				newSampleData->at(i) = sound->getSampleData()->at(i);
			}
			newSample->setMono(sound->isMono());
			lSampler->trimSample(lSampler->getSoundCount() - 1, sound->getStart(), sound->getEnd());
			soundGui->setSoundIndex(lSampler->getSoundCount() - 1, lSampler->getSoundCount());
		}
		else if (editSoundGui->getEdit() == 3) {
			auto source = lSampler->getSound(editSoundGui->getInsertSndNr()).lock();
			auto destination = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
			int destinationStart = sound->getStart();
			int sourceLength = source->getSampleData()->size();
			if (!source->isMono()) sourceLength /= 2;

			int destinationLength = destination->getSampleData()->size();
			if (!destination->isMono()) destinationLength /= 2;

			auto newSampleLength = sourceLength + destinationLength;
			if (!destination->isMono()) newSampleLength *= 2;

			auto sourceData = source->getSampleData();
			vector<float> frame{ 0 };
			if (destination->isMono()) {
				for (int i = destinationStart; i < destinationStart + sourceLength; i++) {
					frame[0] = sourceData->at(i - destinationStart);
					destination->insertFrame(frame, i);
				}
			}
			else  {
				frame.resize(2);
				for (int i = destinationStart; i < destinationStart + sourceLength; i++) {
					frame[0] = sourceData->at(i - destinationStart);
					if (source->isMono()) {
						frame[1] = frame[0];
					}
					else {
						frame[1] = sourceData->at(i - destinationStart + (source->getLastFrameIndex() + 1));
					}
					destination->insertFrame(frame, i);
				}
			}
			destination->setStart(0);
			destination->setMono(destination->isMono());
			destination->setLoopTo(destination->getEnd());
		}
		else if (editSoundGui->getEdit() == 4) {
			lSampler->deleteSection(soundGui->getSoundIndex(), sound->getStart(), sound->getEnd());
			sound->setStart(0);
			sound->setEnd(sound->getLastFrameIndex() + 1);
			sound->setLoopTo(sound->getLastFrameIndex() + 1);
		}
		else if (editSoundGui->getEdit() == 5) {
			auto start = sound->getStart();
			auto end = sound->getEnd();
			for (int i = start; i < end; i++) {
				sound->getSampleData()->at(i) = 0.0f;
				if (!sound->isMono()) sound->getSampleData()->at(i + sound->getLastFrameIndex() + 1) = 0.0f;
			}
			lLs->openScreen(editSoundGui->getPreviousScreenName());
			break;
		}
		else if (editSoundGui->getEdit() == 6) {
			auto start = sound->getStart();
			auto end = sound->getEnd();
			auto reverseCounter = end - 1;
			auto newSampleData = vector<float>(sound->getSampleData()->size());
			if (!sound->isMono()) {
				vector<float> newSampleDataLeft(sound->getSampleData()->size() / 2);
				vector<float> newSampleDataRight(sound->getSampleData()->size() / 2);
				auto sampleData = sound->getSampleData();
				for (int i = 0; i < start; i++) {
					newSampleDataLeft[i] = (*sampleData)[i];
					newSampleDataRight[i] = (*sampleData)[i + sound->getLastFrameIndex()];
				}
				for (int i = start; i < end; i++) {
					newSampleDataLeft[i] = (*sampleData)[reverseCounter];
					newSampleDataRight[i] = (*sampleData)[reverseCounter + sound->getLastFrameIndex()];
					reverseCounter--;
				}
				for (int i = end; i < sound->getLastFrameIndex(); i++) {
					newSampleDataLeft[i] = (*sampleData)[i];
					newSampleDataRight[i] = (*sampleData)[i + sound->getLastFrameIndex()];
				}
				newSampleData = mpc::sampler::Sampler::mergeToStereo(newSampleDataLeft, newSampleDataRight);
			}
			else {
				auto sampleData = sound->getSampleData();
				for (int i = start; i < end; i++) {
					newSampleData[i] = (*sampleData)[reverseCounter];
					reverseCounter--;
				}
			}
			sound->getSampleData()->swap(newSampleData);
			sound->setMono(sound->isMono());
			lLs->openScreen(editSoundGui->getPreviousScreenName());
			break;
		}
		else if (editSoundGui->getEdit() == 7) {
			if (editSoundGui->getTimeStretchRatio() == 10000)
				return;

			if (!sound->isMono()) {
				vector<float> sampleDataLeft = (*sound->getSampleData());
				sampleDataLeft.erase(sampleDataLeft.begin() + (sampleDataLeft.size() / 2), sampleDataLeft.end());
				vector<float> sampleDataRight = (*sound->getSampleData());
				sampleDataRight.erase(sampleDataRight.begin(), sampleDataRight.begin() + (sampleDataRight.size() / 2));
				auto ts0 = mpc::sampler::TimeStretch(sampleDataLeft, (float)(editSoundGui->getTimeStretchRatio() / 10000.0), sound->getSampleRate(), editSoundGui->getTimeStretchAdjust());
				auto newSampleDataLeft = ts0.getProcessedData();
				auto ts1 = mpc::sampler::TimeStretch(sampleDataRight, (float)(editSoundGui->getTimeStretchRatio() / 10000.0), sound->getSampleRate(), editSoundGui->getTimeStretchAdjust());
				auto newSampleDataRight = ts1.getProcessedData();
				auto newSample = lSampler->addSound(sound->getSampleRate()).lock();
				auto newSampleData = mpc::sampler::Sampler::mergeToStereo(newSampleDataLeft, newSampleDataRight);
				auto newSampleDataP = newSample->getSampleData();
				newSampleDataP->swap(newSampleData);
				newSample->setMono(false);
				newSample->setName(editSoundGui->getNewName());
			}
			if (sound->isMono()) {
				auto ts = mpc::sampler::TimeStretch(*sound->getSampleData(), (float)(editSoundGui->getTimeStretchRatio() / 10000.0), sound->getSampleRate(), editSoundGui->getTimeStretchAdjust());
				auto newSample = lSampler->addSound(sound->getSampleRate()).lock();
                auto procData = ts.getProcessedData();
				newSample->getSampleData()->swap(procData);
				newSample->setMono(true);
				newSample->setName(editSoundGui->getNewName());
			}
		}	
		else if (editSoundGui->getEdit() == 8) {
			auto endMargin = editSoundGui->getEndMargin();
			auto source = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock());
			for (int i = 0; i < soundGui->getNumberOfZones(); i++) {
				auto start = soundGui->getZoneStart(i);
				auto end = soundGui->getZoneEnd(i);
				if (i == soundGui->getNumberOfZones() - 1)
					endMargin = 0;

				auto zone = lSampler->createZone(source, start, end, endMargin);
				zone.lock()->setName(editSoundGui->getNewName() + to_string(i+1));
			}
			soundGui->setSoundIndex(lSampler->getSoundCount() - soundGui->getNumberOfZones(), lSampler->getSoundCount());
			soundGui->initZones(lSampler->getSound(soundGui->getSoundIndex()).lock()->getLastFrameIndex());
			if (editSoundGui->getCreateNewProgram()) {
				auto p = lSampler->addProgram().lock();
				p->setName(source->getName());
				for (int i = 0; i < soundGui->getNumberOfZones(); i++) {
					auto pad = p->getPad(i);
					auto note = pad->getNote();
					auto n = p->getNoteParameters(note);
					dynamic_cast<mpc::sampler::NoteParameters*>(n)->setSoundNumber(lSampler->getSoundCount() - soundGui->getNumberOfZones() + i);
				}
				auto lSequencer = sequencer.lock();
				auto s = lSequencer->getSequence(lSequencer->getActiveSequenceIndex()).lock();
				auto t = s->getTrack(lSequencer->getActiveTrackIndex()).lock();
				if (t->getBusNumber() != 0) {
					lSampler->getDrum(t->getBusNumber() - 1)->setProgram(lSampler->getProgramCount() - 1);
				}
			}
		}
		soundGui->setSoundIndex(lSampler->getSoundCount() - 1, lSampler->getSoundCount());
		soundGui->initZones(lSampler->getSound(soundGui->getSoundIndex()).lock()->getLastFrameIndex());
		lLs->openScreen(editSoundGui->getPreviousScreenName());
		break;
	}
}
