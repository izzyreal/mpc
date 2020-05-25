#include <controls/sampler/window/EditSoundControls.hpp>

#include <Mpc.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/ZoneScreen.hpp>

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
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

EditSoundControls::EditSoundControls() 
	: AbstractSamplerControls()
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
	AbstractSamplerControls::function(j);
	
	switch (j)
	{

	case 4:
		auto sound = sampler.lock()->getSound().lock();
		
		if (editSoundGui->getEdit() == 0)
		{
			auto newLength = sound->getEnd() - sound->getStart();

			if (sound->getStart() > sound->getLoopTo())
			{
				if (sound->getLoopTo() > newLength)
				{
					sound->setLoopTo(newLength);
				}
				else
				{
					sound->setLoopTo(0);
				}
			}
			else
			{
				sound->setLoopTo(sound->getLoopTo() - sound->getStart());
			}
			
			sampler.lock()->trimSample(sampler.lock()->getSoundIndex(), sound->getStart(), sound->getEnd());
			
			sound->setStart(0);
			sound->setEnd(newLength);
			sound->setMono(sound->isMono());
		}
		else if (editSoundGui->getEdit() == 1)
		{
			sound->setLoopTo(sound->getStart());
		}
		else if (editSoundGui->getEdit() == 2)
		{
			auto newSample = sampler.lock()->addSound().lock();
			newSample->setSampleRate(sound->getSampleRate());
			newSample->setName(editSoundGui->getNewName());
			auto newSampleData = newSample->getSampleData();
			newSampleData->resize(sound->getSampleData()->size());
			
			for (int i = 0; i < newSampleData->size(); i++)
			{
				newSampleData->at(i) = sound->getSampleData()->at(i);
			}
			
			newSample->setMono(sound->isMono());
			sampler.lock()->trimSample(sampler.lock()->getSoundCount() - 1, sound->getStart(), sound->getEnd());
			sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		}
		else if (editSoundGui->getEdit() == 3)
		{
			// Insert sound into section start
			auto source = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(editSoundGui->getInsertSndNr()).lock());
			auto destination = sampler.lock()->getSound().lock();
			
			int destinationStartFrame = sound->getStart();
	
			int sourceFrameCount = source->getFrameCount();
			int destinationFrameCount = destination->getFrameCount();
			
			auto newSoundFrameCount = sourceFrameCount + destinationFrameCount;
			auto newSoundSampleCount = newSoundFrameCount;
			
			auto sourceData = source->getSampleData();
			auto destinationData = destination->getSampleData();

			if (!destination->isMono())
			{
				newSoundSampleCount *= 2;
			}

			vector<float> newData(newSoundSampleCount);
			int sourceFrameCounter = 0;
			int destinationFrameCounter = 0;

			for (int i = 0; i < newSoundFrameCount; i++)
			{
				if (i < destinationStartFrame)
				{
					newData[i] = (*destinationData)[destinationFrameCounter];

					if (!destination->isMono())
					{
						newData[i + newSoundFrameCount] = (*destinationData)[destinationFrameCounter + destinationFrameCount];
					}
					destinationFrameCounter++;
				}
				else if (i < destinationStartFrame + sourceFrameCount)
				{
					newData[i] = (*sourceData)[sourceFrameCounter];
				
					if (!destination->isMono())
					{
						if (source->isMono())
						{
							newData[i + newSoundFrameCount] = newData[i];
						}
						else
						{
							newData[i + newSoundFrameCount] = (*sourceData)[sourceFrameCounter + sourceFrameCount];
						}
						
					}
					sourceFrameCounter++;
				}
				else
				{
					newData[i] = (*destinationData)[destinationFrameCounter];
					if (!destination->isMono())
					{
						newData[i + newSoundFrameCount] = (*destinationData)[destinationFrameCounter + destinationFrameCount];
					}
					destinationFrameCounter++;
				}
			}
			
			(*destination->getSampleData()) = newData;
			destination->setEnd(newSoundFrameCount);
			destination->setStart(0);
			destination->setMono(destination->isMono());
			destination->setLoopTo(destination->getEnd());
		}
		else if (editSoundGui->getEdit() == 4)
		{
			sampler.lock()->deleteSection(sampler.lock()->getSoundIndex(), sound->getStart(), sound->getEnd());
			sound->setStart(0);
			sound->setEnd(sound->getFrameCount());
			sound->setLoopTo(sound->getFrameCount());
		}
		else if (editSoundGui->getEdit() == 5)
 {
			auto start = sound->getStart();
			auto end = sound->getEnd();

			for (int i = start; i < end; i++)
			{
				sound->getSampleData()->at(i) = 0.0f;

				if (!sound->isMono())
				{
					sound->getSampleData()->at(i + sound->getFrameCount()) = 0.0f;
				}
			}
			ls.lock()->openScreen(editSoundGui->getPreviousScreenName());
			break;
		}
		else if (editSoundGui->getEdit() == 6)
		{
			auto start = sound->getStart();
			auto end = sound->getEnd();
			auto reverseCounter = end - 1;
			auto newSampleData = vector<float>(sound->getSampleData()->size());
		
			if (!sound->isMono())
			{
				vector<float> newSampleDataLeft(sound->getSampleData()->size() * 0.5);
				vector<float> newSampleDataRight(sound->getSampleData()->size() * 0.5);
				auto sampleData = sound->getSampleData();

				for (int i = 0; i < start; i++)
				{
					newSampleDataLeft[i] = (*sampleData)[i];
					newSampleDataRight[i] = (*sampleData)[i + sound->getFrameCount()];
				}
				
				for (int i = start; i < end; i++)
				{
					newSampleDataLeft[i] = (*sampleData)[reverseCounter];
					newSampleDataRight[i] = (*sampleData)[reverseCounter + sound->getFrameCount()];
					reverseCounter--;
				}

				for (int i = end; i < sound->getFrameCount(); i++)
				{
					newSampleDataLeft[i] = (*sampleData)[i];
					newSampleDataRight[i] = (*sampleData)[i + sound->getFrameCount()];
				}
				newSampleData = mpc::sampler::Sampler::mergeToStereo(newSampleDataLeft, newSampleDataRight);
			}
			else
			{
				auto sampleData = sound->getSampleData();
				
				for (int i = start; i < end; i++)
				{
					newSampleData[i] = (*sampleData)[reverseCounter];
					reverseCounter--;
				}
			}
			sound->getSampleData()->swap(newSampleData);
			sound->setMono(sound->isMono());
			ls.lock()->openScreen(editSoundGui->getPreviousScreenName());
			break;
		}
		else if (editSoundGui->getEdit() == 7)
 {
			if (editSoundGui->getTimeStretchRatio() == 10000)
			{
				return;
			}

			if (!sound->isMono())
			{
				vector<float> sampleDataLeft = (*sound->getSampleData());
				sampleDataLeft.erase(sampleDataLeft.begin() + (sampleDataLeft.size() * 0.5), sampleDataLeft.end());
				vector<float> sampleDataRight = (*sound->getSampleData());
				sampleDataRight.erase(sampleDataRight.begin(), sampleDataRight.begin() + (sampleDataRight.size() * 0.5));
				
				auto ts0 = mpc::sampler::TimeStretch(sampleDataLeft, (float)(editSoundGui->getTimeStretchRatio() / 10000.0), sound->getSampleRate(), editSoundGui->getTimeStretchAdjust());
				auto newSampleDataLeft = ts0.getProcessedData();
				auto ts1 = mpc::sampler::TimeStretch(sampleDataRight, (float)(editSoundGui->getTimeStretchRatio() / 10000.0), sound->getSampleRate(), editSoundGui->getTimeStretchAdjust());
				auto newSampleDataRight = ts1.getProcessedData();
				auto newSample = sampler.lock()->addSound(sound->getSampleRate()).lock();
				auto newSampleData = mpc::sampler::Sampler::mergeToStereo(newSampleDataLeft, newSampleDataRight);
				auto newSampleDataP = newSample->getSampleData();
				newSampleDataP->swap(newSampleData);
				newSample->setMono(false);
				newSample->setName(editSoundGui->getNewName());
			}

			if (sound->isMono())
			{
				auto ts = mpc::sampler::TimeStretch(*sound->getSampleData(), (float)(editSoundGui->getTimeStretchRatio() / 10000.0), sound->getSampleRate(), editSoundGui->getTimeStretchAdjust());
				auto newSample = sampler.lock()->addSound(sound->getSampleRate()).lock();
                auto procData = ts.getProcessedData();
				newSample->getSampleData()->swap(procData);
				newSample->setMono(true);
				newSample->setName(editSoundGui->getNewName());
			}
		}	
		else if (editSoundGui->getEdit() == 8)
 {
			auto endMargin = editSoundGui->getEndMargin();
			auto source = sampler.lock()->getSound().lock();

			auto zoneScreen = dynamic_pointer_cast<ZoneScreen>(Screens::getScreenComponent("zone"));
			auto zoneCount = zoneScreen->numberOfZones;

			for (int i = 0; i < zoneScreen->numberOfZones; i++)
			{
				auto start = zoneScreen->getZoneStart(i);
				auto end = zoneScreen->getZoneEnd(i);
			
				if (i == zoneScreen->numberOfZones - 1)
				{
					endMargin = 0;
				}

				auto zone = sampler.lock()->createZone(source, start, end, endMargin);
				zone.lock()->setName(editSoundGui->getNewName() + to_string(i+1));
			}

			sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - zoneCount);

			if (editSoundGui->getCreateNewProgram())
			{
				auto p = sampler.lock()->addProgram().lock();
				p->setName(source->getName());
			
				for (int i = 0; i < zoneCount; i++)
				{
					auto pad = p->getPad(i);
					auto note = pad->getNote();
					auto n = p->getNoteParameters(note);
					dynamic_cast<mpc::sampler::NoteParameters*>(n)->setSoundNumber(sampler.lock()->getSoundCount() - zoneCount + i);
				}
				
				auto s = sequencer.lock()->getSequence(sequencer.lock()->getActiveSequenceIndex()).lock();
				auto t = s->getTrack(sequencer.lock()->getActiveTrackIndex()).lock();

				if (t->getBusNumber() != 0)
				{
					sampler.lock()->getDrum(t->getBusNumber() - 1)->setProgram(sampler.lock()->getProgramCount() - 1);
				}
			}
		}
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		ls.lock()->openScreen(editSoundGui->getPreviousScreenName());
		break;
	}
}
