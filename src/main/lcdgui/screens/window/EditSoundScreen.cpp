#include "EditSoundScreen.hpp"

#include <sampler/TimeStretch1.hpp>
#include <sampler/Pad.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

EditSoundScreen::EditSoundScreen(const int layerIndex)
    : ScreenComponent("edit-sound", layerIndex)
{
	vector<string> newTimeStretchPresetNames = vector<string>(54);
	
	const auto letters = vector<string>{ "A", "B", "C" };
	
    for (auto& s : timeStretchPresetNames)
    {
		for (int i = 0; i < 3; i++)
        {
			s = StrUtil::padRight(s, " ", 13);
			s += letters[i];
			newTimeStretchPresetNames.push_back(s);
		}
	}

	timeStretchPresetNames = newTimeStretchPresetNames;
}

void EditSoundScreen::open()
{
	if (ls.lock()->getPreviousScreenName().compare("name") != 0 && sampler.lock()->getSound().lock())
	{
		auto newSoundName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSoundName = sampler.lock()->addOrIncreaseNumber(newSoundName);
		setNewName(newSoundName);
	}
	displayEdit();
}

void EditSoundScreen::displayEdit()
{
    findField("edit").lock()->setText(editNames[edit]);
    
    if (edit == 0)
    {
        findBackground().lock()->setName("edit-sound");
		findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
    }
    else if (edit == 1)
    {
        findBackground().lock()->setName("editloopfromsttoend");
		findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
    }
    else if (edit == 2)
    {
        findBackground().lock()->setName("editempty");
		findField("new-name").lock()->Hide(false);
        findLabel("new-name").lock()->Hide(false);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
        displayVariable();
    }
    else if (edit == 3)
    {
        findBackground().lock()->setName("editempty");
		findField("new-name").lock()->Hide(false);
        findLabel("new-name").lock()->Hide(false);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
        displayVariable();
    }
    else if (edit == 4)
    {
        findBackground().lock()->setName("editexecute");
		findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
    }
    else if (edit == 5)
    {
        findBackground().lock()->setName("editexecute");
		findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
    }
    else if (edit == 6)
    {
        findBackground().lock()->setName("editexecute");
        findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
    }
    else if (edit == 7)
    {
        findBackground().lock()->setName("editempty");
		findField("new-name").lock()->Hide(false);
        findLabel("new-name").lock()->Hide(false);
        findField("ratio").lock()->Hide(false);
        findLabel("ratio").lock()->Hide(false);
        findField("preset").lock()->Hide(false);
        findLabel("preset").lock()->Hide(false);
        findField("adjust").lock()->Hide(false);
        findLabel("adjust").lock()->Hide(false);
        findLabel("end-margin").lock()->Hide(true);
        findField("end-margin").lock()->Hide(true);
        findLabel("create-new-program").lock()->Hide(true);
        findField("create-new-program").lock()->Hide(true);
        displayVariable();
    }
    else if(edit == 8)
    {
        findBackground().lock()->setName("editempty");
		findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findLabel("end-margin").lock()->Hide(false);
        findField("end-margin").lock()->Hide(false);
        findLabel("create-new-program").lock()->Hide(false);
        findField("create-new-program").lock()->Hide(false);
        displayEndMargin();
        displayCreateNewProgram();
    }
	//ls.lock()->setFunctionKeysArrangement(0); What happens if we draw a background? Will the function keys get drawn again?
}

void EditSoundScreen::displayCreateNewProgram()
{
    findField("create-new-program").lock()->setText(createNewProgram ? "YES" : "NO");
}

void EditSoundScreen::displayEndMargin()
{
    findField("end-margin").lock()->setText(to_string(endMargin));
}

void EditSoundScreen::displayVariable()
{	
    if (edit == 2)
    {
		findLabel("new-name").lock()->setSize(9 * 6 * 2, 18);
		findLabel("new-name").lock()->setText("New name:");
		findField("new-name").lock()->setLocation((findLabel("new-name").lock()->getW() * 0.5) + 19, 21 - 2);
		findField("new-name").lock()->setText(newName);
	}
	else if (edit == 3)
    {
		auto sampleName = sampler.lock()->getSoundName(insertSoundNumber);
		findLabel("new-name").lock()->setSize(11 * 6 * 2, 18);
		findLabel("new-name").lock()->setText("Insert Snd:");
		findField("new-name").lock()->setLocation((findLabel("new-name").lock()->getW() * 0.5) + 19 * 2, 21 - 2);
		string stereo = "";
		if (!sampler.lock()->getSound(insertSoundNumber).lock()->isMono()) stereo = "(ST)";
		findField("new-name").lock()->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
	}
	else if (edit == 7) {
		findLabel("new-name").lock()->setSize(9 * 6 * 2, 18);
		findLabel("new-name").lock()->setText("New name:");
		findField("new-name").lock()->setLocation((findLabel("new-name").lock()->getW() * 0.5) + (19 * 2), 21 - 2);
		findField("new-name").lock()->setText(newName);
		findField("ratio").lock()->setText(to_string(timeStretchRatio * 0.01) + "%");
		findField("preset").lock()->setText(timeStretchPresetNames[timeStretchPresetNumber]);
		findField("adjust").lock()->setText(to_string(timeStretchAdjust));
	}
}

void EditSoundScreen::setNewName(string s)
{
    newName = s;
}

void EditSoundScreen::setEdit(int i)
{
    if (i < 0 || i > 8)
    {
        return;
    }
    
    if (previousScreenName.compare("zone") != 0 && i > 7)
    {
        return;
    }

    edit = i;
    displayEdit();
}

void EditSoundScreen::setInsertSndNr(int i, int soundCount)
{
    if (i < 0 || i > soundCount - 1)
    {
        return;
    }

    insertSoundNumber = i;
    displayEdit();
}

void EditSoundScreen::setTimeStretchRatio(int i)
{
    if (i < 5000 || i > 20000)
    {
        return;
    }

    timeStretchRatio = i;
    
    displayEdit();
}

void EditSoundScreen::setTimeStretchPresetNumber(int i)
{
    if (i < 0 || i > 53)
    {
        return;
    }

    timeStretchPresetNumber = i;
    displayEdit();
}

void EditSoundScreen::setTimeStretchAdjust(int i)
{
    if (i < -100 || i > 100)
    {
        return;
    }

    timeStretchAdjust = i;
    displayEdit();
}

void EditSoundScreen::setPreviousScreenName(string s)
{
    previousScreenName = s;
}

string EditSoundScreen::getPreviousScreenName()
{
    return previousScreenName;
}

void EditSoundScreen::setNewName(int i, string s)
{
    newNames.insert(newNames.begin() + i, s);
}

void EditSoundScreen::setCreateNewProgram(bool b)
{
    createNewProgram = b;
    displayCreateNewProgram();
}

void EditSoundScreen::setEndMargin(int i)
{
    if (i < 0 || i > 99)
    {
        return;
    }
    endMargin = i;
    displayEndMargin();
}

void EditSoundScreen::turnWheel(int i)
{
	init();

	if (param.compare("edit") == 0)
	{
		setEdit(edit + i);
	}
	else if (param.compare("new-name") == 0 && (edit == 2 || edit == 7))
	{
		auto nameScreen = dynamic_pointer_cast<NameScreen>(Screens::getScreenComponent("name"));
		nameScreen->setName(findField("new-name").lock()->getText());
		nameScreen->parameterName =  "newname";

		ls.lock()->openScreen("name");
	}
	else if (param.compare("new-name") == 0 && edit == 3)
	{
		setInsertSndNr(insertSoundNumber + i, sampler.lock()->getSoundCount());
	}
	else if (param.compare("ratio") == 0)
	{
		setTimeStretchRatio(timeStretchRatio + i);
	}
	else if (param.compare("preset") == 0)
	{
		setTimeStretchPresetNumber(timeStretchPresetNumber + i);
	}
	else if (param.compare("adjust") == 0)
	{
		setTimeStretchAdjust(timeStretchAdjust + i);
	}
	else if (param.compare("end-margin") == 0)
	{
		setEndMargin(endMargin + i);
	}
	else if (param.compare("create-new-program") == 0)
	{
		setCreateNewProgram(true);
	}
}

void EditSoundScreen::function(int j)
{
	BaseControls::function(j);

	switch (j)
	{
	case 4:
		auto sound = sampler.lock()->getSound().lock();

		if (edit == 0)
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
		else if (edit == 1)
		{
			sound->setLoopTo(sound->getStart());
		}
		else if (edit == 2)
		{
			auto newSample = sampler.lock()->addSound().lock();
			newSample->setSampleRate(sound->getSampleRate());
			newSample->setName(newName);
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
		else if (edit == 3)
		{
			// Insert sound into section start
			auto source = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(insertSoundNumber).lock());
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
		else if (edit == 4)
		{
			sampler.lock()->deleteSection(sampler.lock()->getSoundIndex(), sound->getStart(), sound->getEnd());
			sound->setStart(0);
			sound->setEnd(sound->getFrameCount());
			sound->setLoopTo(sound->getFrameCount());
		}
		else if (edit == 5)
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
			ls.lock()->openScreen(previousScreenName);
			break;
		}
		else if (edit == 6)
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
			ls.lock()->openScreen(previousScreenName);
			break;
		}
		else if (edit == 7)
		{
			if (timeStretchRatio == 10000)
			{
				return;
			}

			if (!sound->isMono())
			{
				vector<float> sampleDataLeft = (*sound->getSampleData());
				sampleDataLeft.erase(sampleDataLeft.begin() + (sampleDataLeft.size() * 0.5), sampleDataLeft.end());
				vector<float> sampleDataRight = (*sound->getSampleData());
				sampleDataRight.erase(sampleDataRight.begin(), sampleDataRight.begin() + (sampleDataRight.size() * 0.5));

				auto ts0 = mpc::sampler::TimeStretch(sampleDataLeft, (float)(timeStretchRatio / 10000.0), sound->getSampleRate(), timeStretchAdjust);
				auto newSampleDataLeft = ts0.getProcessedData();
				auto ts1 = mpc::sampler::TimeStretch(sampleDataRight, (float)(timeStretchRatio / 10000.0), sound->getSampleRate(), timeStretchAdjust);
				auto newSampleDataRight = ts1.getProcessedData();
				auto newSample = sampler.lock()->addSound(sound->getSampleRate()).lock();
				auto newSampleData = mpc::sampler::Sampler::mergeToStereo(newSampleDataLeft, newSampleDataRight);
				auto newSampleDataP = newSample->getSampleData();
				newSampleDataP->swap(newSampleData);
				newSample->setMono(false);
				newSample->setName(newName);
			}

			if (sound->isMono())
			{
				auto ts = mpc::sampler::TimeStretch(*sound->getSampleData(), (float)(timeStretchRatio * 0.0001), sound->getSampleRate(), timeStretchAdjust);
				auto newSample = sampler.lock()->addSound(sound->getSampleRate()).lock();
				auto procData = ts.getProcessedData();
				newSample->getSampleData()->swap(procData);
				newSample->setMono(true);
				newSample->setName(newName);
			}
		}
		else if (edit == 8)
		{
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
				zone.lock()->setName(newName + to_string(i + 1));
			}

			sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - zoneCount);

			if (createNewProgram)
			{
				auto p = sampler.lock()->addProgram().lock();
				p->setName(source->getName());

				for (int i = 0; i < zoneCount; i++)
				{
					auto pad = p->getPad(i);
					auto note = pad->getNote();
					auto noteParameters = p->getNoteParameters(note);
					dynamic_cast<mpc::sampler::NoteParameters*>(noteParameters)->setSoundNumber(sampler.lock()->getSoundCount() - zoneCount + i);
				}

				auto s = sequencer.lock()->getSequence(sequencer.lock()->getActiveSequenceIndex()).lock();
				auto t = s->getTrack(sequencer.lock()->getActiveTrackIndex()).lock();

				if (t->getBus() != 0)
				{
					mpc.getDrum(t->getBus() - 1)->setProgram(sampler.lock()->getProgramCount() - 1);
				}
			}
		}
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		ls.lock()->openScreen(previousScreenName);
		break;
	}
}
