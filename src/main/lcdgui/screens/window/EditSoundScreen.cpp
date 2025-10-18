#include "EditSoundScreen.hpp"
#include "sampler/Pad.hpp"

#include <sampler/TimeStretch1.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/screens/ZoneScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;

EditSoundScreen::EditSoundScreen(mpc::Mpc& mpc, const int layerIndex)
    : ScreenComponent(mpc, "edit-sound", layerIndex)
{
	std::vector<std::string> newTimeStretchPresetNames;
	
	const std::vector<std::string> letters{ "A", "B", "C" };
	
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
	auto previous = ls->getPreviousScreenName();
	findField("create-new-program")->setAlignment(Alignment::Centered);

	if (previous != "name" && sampler->getSound())
	{
		auto newSoundName = sampler->getSound()->getName();
		newSoundName = sampler->addOrIncreaseNumber(newSoundName);
		setNewName(newSoundName);
	}

	if (previous == "zone")
		setEdit(9);
	else if (previous == "loop")
		setEdit(1);
	else if (previous != "name")
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
	findField("edit")->setText(editNames[edit]);

    if (edit == 0)
    {
        findBackground()->setName("edit-sound");
		findField("new-name")->Hide(true);
        findLabel("new-name")->Hide(true);
        findField("ratio")->Hide(true);
        findLabel("ratio")->Hide(true);
        findField("preset")->Hide(true);
        findLabel("preset")->Hide(true);
        findField("adjust")->Hide(true);
        findLabel("adjust")->Hide(true);
        findLabel("end-margin")->Hide(true);
        findField("end-margin")->Hide(true);
        findLabel("create-new-program")->Hide(true);
        findField("create-new-program")->Hide(true);
    }
    else if (edit == 1)
    {
        findBackground()->setName("edit-loop-from-st-to-end");
		findField("new-name")->Hide(true);
        findLabel("new-name")->Hide(true);
        findField("ratio")->Hide(true);
        findLabel("ratio")->Hide(true);
        findField("preset")->Hide(true);
        findLabel("preset")->Hide(true);
        findField("adjust")->Hide(true);
        findLabel("adjust")->Hide(true);
        findLabel("end-margin")->Hide(true);
        findField("end-margin")->Hide(true);
        findLabel("create-new-program")->Hide(true);
        findField("create-new-program")->Hide(true);
    }
    else if (edit == 2 || edit == 3)
    {
        findBackground()->setName("edit-empty");
		findField("new-name")->Hide(false);
        findLabel("new-name")->Hide(false);
        findField("ratio")->Hide(true);
        findLabel("ratio")->Hide(true);
        findField("preset")->Hide(true);
        findLabel("preset")->Hide(true);
        findField("adjust")->Hide(true);
        findLabel("adjust")->Hide(true);
        findLabel("end-margin")->Hide(true);
        findField("end-margin")->Hide(true);
        findLabel("create-new-program")->Hide(true);
        findField("create-new-program")->Hide(true);
    }
    else if (edit == 4 || edit == 5 || edit == 6 || edit == 8)
    {
        findBackground()->setName("edit-execute");
        findField("new-name")->Hide(true);
        findLabel("new-name")->Hide(true);
        findField("ratio")->Hide(true);
        findLabel("ratio")->Hide(true);
        findField("preset")->Hide(true);
        findLabel("preset")->Hide(true);
        findField("adjust")->Hide(true);
        findLabel("adjust")->Hide(true);
        findLabel("end-margin")->Hide(true);
        findField("end-margin")->Hide(true);
        findLabel("create-new-program")->Hide(true);
        findField("create-new-program")->Hide(true);
    }
    else if (edit == 7)
    {
        findBackground()->setName("edit-empty");
		findField("new-name")->Hide(false);
        findLabel("new-name")->Hide(false);
        findField("ratio")->Hide(false);
        findLabel("ratio")->Hide(false);
        findField("preset")->Hide(true);
        findLabel("preset")->Hide(true);
        findField("adjust")->Hide(true);
        findLabel("adjust")->Hide(true);
        findLabel("end-margin")->Hide(true);
        findField("end-margin")->Hide(true);
        findLabel("create-new-program")->Hide(true);
        findField("create-new-program")->Hide(true);
        displayVariable();
    }
    else if(edit == 9)
    {
        findBackground()->setName("edit-empty");
		findField("new-name")->Hide(true);
        findLabel("new-name")->Hide(true);
        findField("ratio")->Hide(true);
        findLabel("ratio")->Hide(true);
        findField("preset")->Hide(true);
        findLabel("preset")->Hide(true);
        findField("adjust")->Hide(true);
        findLabel("adjust")->Hide(true);
        findField("end-margin")->Hide(false);
		findLabel("end-margin")->Hide(false);
		findField("create-new-program")->Hide(false);
		findLabel("create-new-program")->Hide(false);
		displayEndMargin();
        displayCreateNewProgram();
    }

	displayVariable();
}

void EditSoundScreen::displayCreateNewProgram()
{
    findField("create-new-program")->setText(createNewProgram ? "YES" : "NO");
}

void EditSoundScreen::displayEndMargin()
{
    findField("end-margin")->setTextPadded(endMargin);
}

void EditSoundScreen::displayVariable()
{	
    if (edit == 2)
    {
		findLabel("new-name")->setSize(9 * 6, 9);
		findLabel("new-name")->setText("New name:");
		findField("new-name")->setLocation(findLabel("new-name")->getW() + 19, 20);
		findField("new-name")->setText(newName);
	}
	else if (edit == 3)
    {
		auto sampleName = sampler->getSortedSounds()[insertSoundIndex].first->getName();
		findLabel("new-name")->setSize(11 * 6, 9);
		findLabel("new-name")->setText("Insert Snd:");
		findField("new-name")->setLocation(findLabel("new-name")->getW() + 19, 20);
        std::string stereo;
		
		if (!sampler->getSound(insertSoundIndex)->isMono())
		{
			stereo = "(ST)";
		}

		findField("new-name")->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
	}
	else if (edit == 7)
	{
		findLabel("new-name")->setSize(9 * 6, 9);
		findLabel("new-name")->setText("New name:");
		findField("new-name")->setLocation(findLabel("new-name")->getW() + 19, 20); // , 20 is that still from the old days?
		findField("new-name")->setText(newName);

		auto trimmedPercentage = StrUtil::TrimDecimals(std::to_string(timeStretchRatio * 0.01), 2);
		findField("ratio")->setText(StrUtil::padLeft(trimmedPercentage, " ", 6) + "%");

		findField("preset")->setText(timeStretchPresetNames[timeStretchPresetIndex]);
		findField("adjust")->setText(std::to_string(timeStretchAdjust));
	}
}

void EditSoundScreen::setNewName(std::string s)
{
    newName = s;
}

void EditSoundScreen::setEdit(int i)
{
    if (i < 0 || i > 9)
    {
        return;
    }
    
    if (returnToScreenName != "zone" && i > 8)
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

void EditSoundScreen::setReturnToScreenName(std::string s)
{
    returnToScreenName = s;
}

std::string EditSoundScreen::getReturnToScreenName()
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
    
    if (param == "new-name" && (edit == 2 || edit == 7))
        openNameScreen();
    else
        ScreenComponent::right();
}

void EditSoundScreen::openNameScreen()
{
    if (param == "new-name" && (edit == 2 || edit == 7))
    {
        const auto enterAction = [this](std::string &nameScreenName) {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            setNewName(nameScreenName);
            openScreen(name);
        };

        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        nameScreen->initialize(newName, 16, enterAction, name);
        openScreen("name");
    }
}

void EditSoundScreen::turnWheel(int i)
{
	init();

	if (param == "edit")
	{
		setEdit(edit + i);
	}
	else if (param == "new-name" && edit == 3)
	{
		setInsertSndNr(insertSoundIndex + i, sampler->getSoundCount());
	}
	else if (param == "ratio")
	{
		setTimeStretchRatio(timeStretchRatio + i);
	}
	else if (param == "preset")
	{
		setTimeStretchPresetNumber(timeStretchPresetIndex + i);
	}
	else if (param == "adjust")
	{
		setTimeStretchAdjust(timeStretchAdjust + i);
	}
	else if (param == "end-margin")
	{
		setEndMargin(endMargin + i);
	}
	else if (param == "create-new-program")
	{
		setCreateNewProgram(i > 0);
	}
}

static std::shared_ptr<Sound> createZone(
        std::shared_ptr<Sampler> sampler,
        const std::shared_ptr<Sound> source,
        const int start,
        const int end,
        const int endMargin
) {
	const auto overlapInFrames = (int)(endMargin * source->getSampleRate() * 0.001);

    auto zone = sampler->addSound(source->getSampleRate(), "zone");

    if (zone == nullptr)
    {
        return {};
    }

    zone->setMono(source->isMono());

	const auto zoneLength = end - start + overlapInFrames;
	const auto endWithOverlap = start + zoneLength;

    const auto sourceSampleData = source->getSampleData();

    for (uint32_t frameIndex = start; frameIndex < endWithOverlap; ++frameIndex)
    {
        if (frameIndex > source->getLastFrameIndex())
        {
            break;
        }

        const auto frameData = source->isMono() ?
            std::vector<float>{(*sourceSampleData)[frameIndex]} :
            std::vector<float>{(*sourceSampleData)[frameIndex],
                (*sourceSampleData)[frameIndex+source->getLastFrameIndex()]};

        zone->insertFrame(frameData, zone->getLastFrameIndex() + 1);
    }

    zone->setEnd(zone->getFrameCount());

	return zone;
}

void EditSoundScreen::function(int j)
{
	ScreenComponent::function(j);

	auto zoneScreen = mpc.screens->get<ZoneScreen>("zone");

	switch (j)
	{
	case 4:
		auto sound = sampler->getSound();

		if (edit == 0)
		{
			auto end = sound->getEnd();
			auto start = sound->getStart();

			if (returnToScreenName == "loop")
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName == "zone")
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			auto newLoopTo = sound->getLoopTo() - start;

			sampler->trimSample(sampler->getSoundIndex(), start, end);

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
			auto newSample = sampler->addSound(returnToScreenName);

            if (newSample == nullptr)
            {
                return;
            }

			newSample->setSampleRate(sound->getSampleRate());
			newSample->setName(newName);
			auto newSampleData = newSample->getMutableSampleData();
			newSampleData->resize(sound->getSampleData()->size());

			for (int i = 0; i < newSampleData->size(); i++)
            {
                (*newSampleData)[i] = (*sound->getSampleData())[i];
            }

			newSample->setMono(sound->isMono());
			sampler->trimSample(sampler->getSoundCount() - 1, sound->getStart(), sound->getEnd());
			sampler->setSoundIndex(sampler->getSoundCount() - 1);
		}
		else if (edit == 3)
		{
			// Insert sound into section start
			auto source = sampler->getSortedSounds()[insertSoundIndex].first;
			auto destination = sampler->getSound();

			auto destinationStartFrame = sound->getStart();

			if (returnToScreenName == "loop")
			{
				destinationStartFrame = sound->getLoopTo();
			}
			else if (returnToScreenName == "zone")
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

            auto newData = std::make_shared<std::vector<float>>(newSoundSampleCount);
			int sourceFrameCounter = 0;
			int destinationFrameCounter = 0;

			for (int i = 0; i < newSoundFrameCount; i++)
			{
				if (i < destinationStartFrame)
				{
					(*newData)[i] = (*destinationData)[destinationFrameCounter];

					if (!destination->isMono())
						(*newData)[i + newSoundFrameCount] = (*destinationData)[destinationFrameCounter + destinationFrameCount];

					destinationFrameCounter++;
				}
				else if (i < destinationStartFrame + sourceFrameCount)
				{
					(*newData)[i] = (*sourceData)[sourceFrameCounter];

					if (!destination->isMono())
					{
						if (source->isMono())
						{
							(*newData)[i + newSoundFrameCount] = (*newData)[i];
						}
						else
						{
							(*newData)[i + newSoundFrameCount] = (*sourceData)[sourceFrameCounter + sourceFrameCount];
						}

					}
					sourceFrameCounter++;
				}
				else
				{
					(*newData)[i] = (*destinationData)[destinationFrameCounter];

					if (!destination->isMono())
						(*newData)[i + newSoundFrameCount] = (*destinationData)[destinationFrameCounter + destinationFrameCount];

					destinationFrameCounter++;
				}
			}

            destination->setSampleData(newData);
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

			if (returnToScreenName == "loop")
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName == "zone")
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			sampler->deleteSection(sampler->getSoundIndex(), start, end);
			sound->setStart(0);
			sound->setEnd(sound->getFrameCount());
			sound->setLoopTo(sound->getFrameCount());
			zoneScreen->initZones();
		}
		else if (edit == 5)
		{
			auto start = sound->getStart();
			auto end = sound->getEnd();
			
			if (returnToScreenName == "loop")
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName == "zone")
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

            auto newSampleData = *sound->getSampleData();

			for (int i = start; i < end; i++)
			{
				newSampleData[i] = 0.0f;

				if (!sound->isMono())
					newSampleData[(i + sound->getFrameCount())] = 0.0f;
			}

            sound->setSampleData(std::make_shared<std::vector<float>>(newSampleData));

			openScreen(returnToScreenName);
			break;
		}
		else if (edit == 6)
		{
			auto start = sound->getStart();
			auto end = sound->getEnd();

			if (returnToScreenName == "loop")
			{
				start = sound->getLoopTo();
			}
			else if (returnToScreenName == "zone")
			{
				const auto zone = zoneScreen->zone;
				start = zoneScreen->getZoneStart(zone);
				end = zoneScreen->getZoneEnd(zone);
			}

			auto reverseCounter = end - 1;
			auto newSampleData = std::vector<float>(sound->getSampleData()->size());

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
                std::vector<float> newSampleDataLeft(sound->getSampleData()->size() * 0.5);
                std::vector<float> newSampleDataRight(sound->getSampleData()->size() * 0.5);
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

			sound->setSampleData(std::make_shared<std::vector<float>>(newSampleData));
			openScreen(returnToScreenName);
			break;
		}
		else if (edit == 7)
		{
			if (timeStretchRatio == 10000)
				return;

			if (sound->isMono())
			{
				auto newSample = sampler->addSound(sound->getSampleRate(), returnToScreenName);

                if (newSample == nullptr)
                {
                    return;
                }

				auto ts = mpc::sampler::TimeStretch(*sound->getSampleData(), (float)(timeStretchRatio * 0.0001), sound->getSampleRate(), timeStretchAdjust);
				auto &procData = ts.getProcessedData();
				newSample->setSampleData(std::make_shared<std::vector<float>>(procData));
				newSample->setMono(true);
				newSample->setName(newName);
			}
			else
			{
    	        auto newSound = sampler->addSound(sound->getSampleRate(), returnToScreenName);

                if (newSound == nullptr)
                {
                    return;
                }

                std::vector<float> sampleDataLeft = *sound->getSampleData();
				sampleDataLeft.erase(sampleDataLeft.begin() + (sampleDataLeft.size() * 0.5), sampleDataLeft.end());
                std::vector<float> sampleDataRight = *sound->getSampleData();
				sampleDataRight.erase(sampleDataRight.begin(), sampleDataRight.begin() + (sampleDataRight.size() * 0.5));

				auto ts0 = mpc::sampler::TimeStretch(sampleDataLeft, (float)(timeStretchRatio / 10000.0), sound->getSampleRate(), timeStretchAdjust);
				auto &newSampleDataLeft = ts0.getProcessedData();
				auto ts1 = mpc::sampler::TimeStretch(sampleDataRight, (float)(timeStretchRatio / 10000.0), sound->getSampleRate(), timeStretchAdjust);
				auto &newSampleDataRight = ts1.getProcessedData();
				auto newSampleData = mpc::sampler::Sampler::mergeToStereo(newSampleDataLeft, newSampleDataRight);
				newSound->setSampleData(std::make_shared<std::vector<float>>(newSampleData));
				newSound->setMono(false);
				newSound->setName(newName);
			}
		}
        else if (edit == 8)
        {
            auto start = sound->getStart();
            auto end = sound->getEnd();
            
            if (returnToScreenName == "loop")
            {
                start = sound->getLoopTo();
            }
            else if (returnToScreenName == "zone")
            {
                const auto zone = zoneScreen->zone;
                start = zoneScreen->getZoneStart(zone);
                end = zoneScreen->getZoneEnd(zone);
            }

            float peak = 0.0f;
            auto sampleData = sound->getSampleData();
            
            for (int i = start; i < end; i++)
            {
                float v = std::fabs((*sampleData)[i]);
                peak = std::fmax(peak, v);

                if (!sound->isMono())
                {
                    v = std::fabs((*sampleData)[(i + sound->getFrameCount())]);
                    peak = std::fmax(peak, v);
                }
            }
            
            peak = std::min(1.0f, peak);
            
            const float factor = 1.0f / peak;

            auto newSampleData = *sampleData;

            for (int i = start; i < end; i++)
            {
                newSampleData[i] = newSampleData[i] * factor;

                if (!sound->isMono())
                {
                    newSampleData[(i + sound->getFrameCount())] = newSampleData[i] * factor;
                }
            }

            sound->setSampleData(std::make_shared<std::vector<float>>(newSampleData));
            
            openScreen(returnToScreenName);
            break;
        }
		else if (edit == 9)
		{
			auto source = sampler->getSound();
			auto zoneCount = zoneScreen->numberOfZones;

			for (int i = 0; i < zoneScreen->numberOfZones; i++)
			{
				auto start = zoneScreen->getZoneStart(i);
				auto end = zoneScreen->getZoneEnd(i);

				auto zone = createZone(sampler, source, start, end, endMargin);

                if (zone == nullptr)
                {
                    return;
                }
				
				if (i == 0)
				{
					zone->setName(newName);
				}
				else
				{
					zone->setName(sampler->addOrIncreaseNumber(newName));
				}
			}

			if (createNewProgram)
			{
				auto p = sampler->createNewProgramAddFirstAvailableSlot().lock();
				p->setName(source->getName());

				for (int i = 0; i < zoneCount; i++)
				{
					auto pad = p->getPad(i);
					auto note = pad->getNote();
					auto noteParameters = p->getNoteParameters(note);
					dynamic_cast<mpc::sampler::NoteParameters*>(noteParameters)->setSoundIndex(sampler->getSoundCount() - zoneCount + i);
				}

				auto s = sequencer.lock()->getSequence(sequencer.lock()->getActiveSequenceIndex());
				auto t = sequencer.lock()->getActiveTrack();

				if (t->getBus() != 0)
				{
					mpc.getDrum(t->getBus() - 1).setProgram(sampler->getProgramCount() - 1);
				}
			}
		}

		openScreen(returnToScreenName);
		break;
	}
}
