#include "EditSoundScreen.hpp"

#include <sampler/TimeStretch1.hpp>
#include <sampler/Pad.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

EditSoundScreen::EditSoundScreen(mpc::Mpc& mpc, const int layerIndex)
    : ScreenComponent(mpc, "edit-sound", layerIndex)
{
	vector<string> newTimeStretchPresetNames;
	
	const auto letters = vector<string>{ "A", "B", "C" };
	
    for (auto& s : timeStretchPresetNames)
    {
		for (int i = 0; i < 3; i++)
        {
			auto newStr = StrUtil::padRight(s, " ", 13);
			newStr += letters[i];
			newTimeStretchPresetNames.push_back(newStr);
		}
	}

	timeStretchPresetNames = newTimeStretchPresetNames;
}

void EditSoundScreen::open()
{
	auto previous = ls.lock()->getPreviousScreenName();
	findField("create-new-program").lock()->setAlignment(Alignment::Centered);

	if (previous.compare("name") != 0 && sampler.lock()->getSound().lock())
	{
		auto newSoundName = sampler.lock()->getSound().lock()->getName();
		newSoundName = sampler.lock()->addOrIncreaseNumber(newSoundName);
		setNewName(newSoundName);
	}

	if (previous.compare("zone") == 0)
		setEdit(9);
	else if (previous.compare("loop") == 0)
		setEdit(1);
	else if (previous.compare("name") != 0)
		setEdit(0);

	displayVariable();

	if (edit == 9)
	{
		displayEndMargin();
		displayCreateNewProgram();
	}
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
        findBackground().lock()->setName("edit-loop-from-st-to-end");
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
    else if (edit == 2 || edit == 3)
    {
        findBackground().lock()->setName("edit-empty");
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
    }
    else if (edit == 4 || edit == 5 || edit == 6 || edit == 8)
    {
        findBackground().lock()->setName("edit-execute");
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
        findBackground().lock()->setName("edit-empty");
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
    else if(edit == 9)
    {
        findBackground().lock()->setName("edit-empty");
		findField("new-name").lock()->Hide(true);
        findLabel("new-name").lock()->Hide(true);
        findField("ratio").lock()->Hide(true);
        findLabel("ratio").lock()->Hide(true);
        findField("preset").lock()->Hide(true);
        findLabel("preset").lock()->Hide(true);
        findField("adjust").lock()->Hide(true);
        findLabel("adjust").lock()->Hide(true);
        findField("end-margin").lock()->Hide(false);
		findLabel("end-margin").lock()->Hide(false);
		findField("create-new-program").lock()->Hide(false);
		findLabel("create-new-program").lock()->Hide(false);
		displayEndMargin();
        displayCreateNewProgram();
    }

	displayVariable();
}

void EditSoundScreen::displayCreateNewProgram()
{
    findField("create-new-program").lock()->setText(createNewProgram ? "YES" : "NO");
}

void EditSoundScreen::displayEndMargin()
{
    findField("end-margin").lock()->setTextPadded(endMargin);
}

void EditSoundScreen::displayVariable()
{	
    if (edit == 2)
    {
		findLabel("new-name").lock()->setSize(9 * 6, 9);
		findLabel("new-name").lock()->setText("New name:");
		findField("new-name").lock()->setLocation(findLabel("new-name").lock()->getW() + 19, 20);
		findField("new-name").lock()->setText(newName);
	}
	else if (edit == 3)
    {
		auto sampleName = sampler.lock()->getSoundName(insertSoundIndex);
		findLabel("new-name").lock()->setSize(11 * 6, 9);
		findLabel("new-name").lock()->setText("Insert Snd:");
		findField("new-name").lock()->setLocation(findLabel("new-name").lock()->getW() + 19, 20);
		string stereo = "";
		
		if (!sampler.lock()->getSound(insertSoundIndex).lock()->isMono())
		{
			stereo = "(ST)";
		}

		findField("new-name").lock()->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
	}
	else if (edit == 7)
	{
		findLabel("new-name").lock()->setSize(9 * 6, 9);
		findLabel("new-name").lock()->setText("New name:");
		findField("new-name").lock()->setLocation(findLabel("new-name").lock()->getW() + 19, 20); // , 20 is that still from the old days?
		findField("new-name").lock()->setText(newName);

		auto trimmedPercentage = StrUtil::TrimDecimals(to_string(timeStretchRatio * 0.01), 2);
		findField("ratio").lock()->setText(StrUtil::padLeft(trimmedPercentage, " ", 6) + "%");

		findField("preset").lock()->setText(timeStretchPresetNames[timeStretchPresetIndex]);
		findField("adjust").lock()->setText(to_string(timeStretchAdjust));
	}
}

void EditSoundScreen::setNewName(string s)
{
    newName = s;
}

void EditSoundScreen::setEdit(int i)
{
    if (i < 0 || i > 9)
    {
        return;
    }
    
    if (returnToScreenName.compare("zone") != 0 && i > 8)
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

    insertSoundIndex = i;
	displayVariable();
}

void EditSoundScreen::setTimeStretchRatio(int i)
{
    if (i < 5000 || i > 20000)
    {
        return;
    }

    timeStretchRatio = i;
    
	displayVariable();
}

void EditSoundScreen::setTimeStretchPresetNumber(int i)
{
    if (i < 0 || i > 53)
    {
        return;
    }

    timeStretchPresetIndex = i;
	displayVariable();
}

void EditSoundScreen::setTimeStretchAdjust(int i)
{
    if (i < -100 || i > 100)
        return;

    timeStretchAdjust = i;
	displayVariable();
}

void EditSoundScreen::setReturnToScreenName(string s)
{
    returnToScreenName = s;
}

string EditSoundScreen::getReturnToScreenName()
{
    return returnToScreenName;
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

void EditSoundScreen::right()
{
    init();
    
    if (param.compare("new-name") == 0 && (edit == 2 || edit == 7))
        openNameScreen();
    else
        ScreenComponent::right();
}

void EditSoundScreen::openNameScreen()
{
    auto nameScreen = mpc.screens->get<NameScreen>("name");
    auto editSoundScreen = this;
    nameScreen->setName(newName);

    auto renamer = [editSoundScreen](string& newName1) {
        editSoundScreen->setNewName(newName1);
    };

    nameScreen->setRenamerAndScreenToReturnTo(renamer, "edit-sound");
    
    openScreen("name");
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
        openNameScreen();
	}
	else if (param.compare("new-name") == 0 && edit == 3)
	{
		setInsertSndNr(insertSoundIndex + i, sampler.lock()->getSoundCount());
	}
	else if (param.compare("ratio") == 0)
	{
		setTimeStretchRatio(timeStretchRatio + i);
	}
	else if (param.compare("preset") == 0)
	{
		setTimeStretchPresetNumber(timeStretchPresetIndex + i);
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
	ScreenComponent::function(j);

	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");

	switch (j)
	{
	case 4:
		auto sound = sampler.lock()->getSound().lock();

		if (edit == 0)
		{
			auto end = sound->getEnd();
			auto start = sound->getStart();

			if (returnToScreenName.compare("loop") == 0)
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName.compare("zone") == 0)
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			auto newLoopTo = sound->getLoopTo() - start;

			sampler.lock()->trimSample(sampler.lock()->getSoundIndex(), start, end);

			sound->setStart(0);

			auto newLength = sound->getEnd() - sound->getStart();
			sound->setEnd(newLength);
			sound->setMono(sound->isMono());
			sound->setLoopTo(newLoopTo);
			zoneScreen->initZones();
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
				(*newSampleData)[i] = (*sound->getSampleData())[i];

			newSample->setMono(sound->isMono());
			sampler.lock()->trimSample(sampler.lock()->getSoundCount() - 1, sound->getStart(), sound->getEnd());
			sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		}
		else if (edit == 3)
		{
			// Insert sound into section start
			auto source = sampler.lock()->getSound(insertSoundIndex).lock();
			auto destination = sampler.lock()->getSound().lock();

			auto destinationStartFrame = sound->getStart();

			if (returnToScreenName.compare("loop") == 0)
			{
				destinationStartFrame = sound->getLoopTo();
			}
			else if (returnToScreenName.compare("zone") == 0)
			{
				const auto zone = zoneScreen->zone;
				destinationStartFrame = zoneScreen->getZoneStart(zone);
			}

			auto sourceFrameCount = source->getFrameCount();
			auto destinationFrameCount = destination->getFrameCount();

			auto newSoundFrameCount = sourceFrameCount + destinationFrameCount;
			auto newSoundSampleCount = newSoundFrameCount;

			auto sourceData = source->getSampleData();
			auto destinationData = destination->getSampleData();

			if (!destination->isMono())
				newSoundSampleCount *= 2;

			vector<float> newData(newSoundSampleCount);
			int sourceFrameCounter = 0;
			int destinationFrameCounter = 0;

			for (int i = 0; i < newSoundFrameCount; i++)
			{
				if (i < destinationStartFrame)
				{
					newData[i] = (*destinationData)[destinationFrameCounter];

					if (!destination->isMono())
						newData[i + newSoundFrameCount] = (*destinationData)[destinationFrameCounter + destinationFrameCount];

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
						newData[i + newSoundFrameCount] = (*destinationData)[destinationFrameCounter + destinationFrameCount];

					destinationFrameCounter++;
				}
			}

			(*destination->getSampleData()) = newData;
			destination->setEnd(newSoundFrameCount);
			destination->setStart(0);
			destination->setMono(destination->isMono());
			destination->setLoopTo(destination->getEnd());
			zoneScreen->initZones();
		}
		else if (edit == 4)
		{
			auto end = sound->getEnd();
			auto start = sound->getStart();

			if (returnToScreenName.compare("loop") == 0)
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName.compare("zone") == 0)
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			sampler.lock()->deleteSection(sampler.lock()->getSoundIndex(), start, end);
			sound->setStart(0);
			sound->setEnd(sound->getFrameCount());
			sound->setLoopTo(sound->getFrameCount());
			zoneScreen->initZones();
		}
		else if (edit == 5)
		{
			auto start = sound->getStart();
			auto end = sound->getEnd();
			
			if (returnToScreenName.compare("loop") == 0)
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName.compare("zone") == 0)
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			for (int i = start; i < end; i++)
			{
				(*sound->getSampleData())[i] = 0.0f;

				if (!sound->isMono())
					(*sound->getSampleData())[(i + sound->getFrameCount())] = 0.0f;
			}

			openScreen(returnToScreenName);
			break;
		}
		else if (edit == 6)
		{
			auto start = sound->getStart();
			auto end = sound->getEnd();

			if (returnToScreenName.compare("loop") == 0)
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName.compare("zone") == 0)
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			auto reverseCounter = end - 1;
			auto newSampleData = vector<float>(sound->getSampleData()->size());

			if (sound->isMono())
			{
				auto sampleData = sound->getSampleData();

				for (int i = start; i < end; i++)
				{
					newSampleData[i] = (*sampleData)[reverseCounter];
					reverseCounter--;
				}
			}
			else
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

			sound->getSampleData()->swap(newSampleData);
			openScreen(returnToScreenName);
			break;
		}
		else if (edit == 7)
		{
			if (timeStretchRatio == 10000)
				return;

			if (sound->isMono())
			{
				auto ts = mpc::sampler::TimeStretch(*sound->getSampleData(), (float)(timeStretchRatio * 0.0001), sound->getSampleRate(), timeStretchAdjust);
				auto newSample = sampler.lock()->addSound(sound->getSampleRate()).lock();
				auto procData = ts.getProcessedData();
				newSample->getSampleData()->swap(procData);
				newSample->setMono(true);
				newSample->setName(newName);
			}
			else
			{
				vector<float> sampleDataLeft = *sound->getSampleData();
				sampleDataLeft.erase(sampleDataLeft.begin() + (sampleDataLeft.size() * 0.5), sampleDataLeft.end());
				vector<float> sampleDataRight = *sound->getSampleData();
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
		}
        else if (edit == 8)
        {
            auto source = sampler.lock()->getSound().lock();
            auto start = sound->getStart();
            auto end = sound->getEnd();
            
            if (returnToScreenName.compare("loop") == 0)
            {
                start = sound->getLoopTo();
            }
            else if (returnToScreenName.compare("zone") == 0)
            {
                const auto zone = zoneScreen->zone;
                start = zoneScreen->getZoneStart(zone);
                end = zoneScreen->getZoneEnd(zone);
            }

            float peak = 0.0f;
            
            for (int i = start; i < end; i++)
            {
                float v = abs((*sound->getSampleData())[i]);
                peak = max(peak, v);

                if (!sound->isMono()) {
                    v = (*sound->getSampleData())[(i + sound->getFrameCount())];
                    peak = max(peak, v);
                }
            }
            
            peak = min(1.0f, peak);
            
            float factor = 1.0 / peak;

            for (int i = start; i < end; i++)
            {
                (*sound->getSampleData())[i] *= factor;
                if (!sound->isMono())
                    (*sound->getSampleData())[(i + sound->getFrameCount())] *= factor;
            }
            
            openScreen(returnToScreenName);
            break;
        }
		else if (edit == 9)
		{
			auto source = sampler.lock()->getSound().lock();
			auto zoneCount = zoneScreen->numberOfZones;

			for (int i = 0; i < zoneScreen->numberOfZones; i++)
			{
				auto start = zoneScreen->getZoneStart(i);
				auto end = zoneScreen->getZoneEnd(i);

				auto zone = sampler.lock()->createZone(source, start, end, endMargin);
				
				if (i == 0)
				{
					zone.lock()->setName(newName);
				}
				else
				{
					zone.lock()->setName(sampler.lock()->addOrIncreaseNumber(newName));
				}
			}

			if (createNewProgram)
			{
				auto p = sampler.lock()->addProgram().lock();
				p->setName(source->getName());

				for (int i = 0; i < zoneCount; i++)
				{
					auto pad = p->getPad(i);
					auto note = pad->getNote();
					auto noteParameters = p->getNoteParameters(note);
					dynamic_cast<mpc::sampler::NoteParameters*>(noteParameters)->setSoundIndex(sampler.lock()->getSoundCount() - zoneCount + i);
				}

				auto s = sequencer.lock()->getSequence(sequencer.lock()->getActiveSequenceIndex()).lock();
				auto t = s->getTrack(sequencer.lock()->getActiveTrackIndex()).lock();

				if (t->getBus() != 0)
				{
					mpc.getDrum(t->getBus() - 1)->setProgram(sampler.lock()->getProgramCount() - 1);
				}
			}
		}

		openScreen(returnToScreenName);
		break;
	}
}
