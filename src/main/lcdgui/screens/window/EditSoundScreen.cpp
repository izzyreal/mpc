#include "EditSoundScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Pad.hpp"

#include "sampler/Sampler.hpp"
#include "sampler/TimeStretch1.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Bus.hpp"

#include "lcdgui/screens/ZoneScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::sequencer;

EditSoundScreen::EditSoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "edit-sound", layerIndex)
{
    std::vector<std::string> newTimeStretchPresetNames;

    const std::vector<std::string> letters{"A", "B", "C"};

    for (const auto &s : timeStretchPresetNames)
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
    findField("create-new-program")->setAlignment(Alignment::Centered);

    if (ls.lock()->isPreviousScreenNot({ScreenId::NameScreen}) &&
        sampler.lock()->getSound())
    {
        auto newSoundName = sampler.lock()->getSound()->getName();
        newSoundName = sampler.lock()->addOrIncreaseNumber(newSoundName);
        setNewName(newSoundName);
    }

    if (ls.lock()->isPreviousScreen({ScreenId::ZoneScreen}))
    {
        setEdit(9);
    }
    else if (ls.lock()->isPreviousScreen({ScreenId::LoopScreen}))
    {
        setEdit(1);
    }
    else if (ls.lock()->isPreviousScreenNot({ScreenId::NameScreen}))
    {
        setEdit(0);
    }

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
        findBackground()->setBackgroundName("edit-sound");
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
        findBackground()->setBackgroundName("edit-loop-from-st-to-end");
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
        findBackground()->setBackgroundName("edit-empty");
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
        findBackground()->setBackgroundName("edit-execute");
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
        findBackground()->setBackgroundName("edit-empty");
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
    else if (edit == 9)
    {
        findBackground()->setBackgroundName("edit-empty");
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

void EditSoundScreen::displayCreateNewProgram() const
{
    findField("create-new-program")->setText(createNewProgram ? "YES" : "NO");
}

void EditSoundScreen::displayEndMargin() const
{
    findField("end-margin")->setTextPadded(endMargin);
}

void EditSoundScreen::displayVariable() const
{
    if (edit == 2)
    {
        findLabel("new-name")->setSize(9 * 6, 9);
        findLabel("new-name")->setText("New name:");
        findField("new-name")
            ->setLocation(findLabel("new-name")->getW() + 19, 20);
        findField("new-name")->setText(newName);
    }
    else if (edit == 3)
    {
        const auto sampleName = sampler.lock()
                                    ->getSortedSounds()[insertSoundIndex]
                                    .first->getName();
        findLabel("new-name")->setSize(11 * 6, 9);
        findLabel("new-name")->setText("Insert Snd:");
        findField("new-name")
            ->setLocation(findLabel("new-name")->getW() + 19, 20);
        std::string stereo;

        if (!sampler.lock()->getSound(insertSoundIndex)->isMono())
        {
            stereo = "(ST)";
        }

        findField("new-name")
            ->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
    }
    else if (edit == 7)
    {
        findLabel("new-name")->setSize(9 * 6, 9);
        findLabel("new-name")->setText("New name:");
        findField("new-name")
            ->setLocation(findLabel("new-name")->getW() + 19,
                          20); // , 20 is that still from the old days?
        findField("new-name")->setText(newName);

        const auto trimmedPercentage =
            StrUtil::TrimDecimals(std::to_string(timeStretchRatio * 0.01), 2);
        findField("ratio")->setText(
            StrUtil::padLeft(trimmedPercentage, " ", 6) + "%");

        findField("preset")->setText(
            timeStretchPresetNames[timeStretchPresetIndex]);
        findField("adjust")->setText(std::to_string(timeStretchAdjust));
    }
}

void EditSoundScreen::setNewName(const std::string &s)
{
    newName = s;
}

void EditSoundScreen::setEdit(const int i)
{
    edit = std::clamp(i, 0, returnToScreenName == "zone" ? 9 : 8);
    displayEdit();
}

void EditSoundScreen::setInsertSndNr(const int i, const int soundCount)
{
    insertSoundIndex = std::clamp(i, 0, soundCount - 1);
    displayVariable();
}

void EditSoundScreen::setTimeStretchRatio(const int i)
{
    timeStretchRatio = std::clamp(i, 5000, 20000);
    displayVariable();
}

void EditSoundScreen::setTimeStretchPresetNumber(const int i)
{
    timeStretchPresetIndex = std::clamp(i, 0, 53);
    displayVariable();
}

void EditSoundScreen::setTimeStretchAdjust(const int i)
{
    timeStretchAdjust = std::clamp(i, -100, 100);
    displayVariable();
}

void EditSoundScreen::setReturnToScreenName(const std::string &s)
{
    returnToScreenName = s;
}

std::string EditSoundScreen::getReturnToScreenName()
{
    return returnToScreenName;
}

void EditSoundScreen::setCreateNewProgram(const bool b)
{
    createNewProgram = b;
    displayCreateNewProgram();
}

void EditSoundScreen::setEndMargin(const int i)
{
    endMargin = std::clamp(i, 0, 99);
    displayEndMargin();
}

void EditSoundScreen::right()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "new-name" && (edit == 2 || edit == 7))
    {
        openNameScreen();
    }
    else
    {
        ScreenComponent::right();
    }
}

void EditSoundScreen::openNameScreen()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "new-name" && (edit == 2 || edit == 7))
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            setNewName(nameScreenName);
            openScreenById(ScreenId::EditSoundScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(newName, 16, enterAction, "edit-sound");
        openScreenById(ScreenId::NameScreen);
    }
}

void EditSoundScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "edit")
    {
        setEdit(edit + increment);
    }
    else if (focusedFieldName == "new-name" && edit == 3)
    {
        setInsertSndNr(insertSoundIndex + increment,
                       sampler.lock()->getSoundCount());
    }
    else if (focusedFieldName == "ratio")
    {
        setTimeStretchRatio(timeStretchRatio + increment);
    }
    else if (focusedFieldName == "preset")
    {
        setTimeStretchPresetNumber(timeStretchPresetIndex + increment);
    }
    else if (focusedFieldName == "adjust")
    {
        setTimeStretchAdjust(timeStretchAdjust + increment);
    }
    else if (focusedFieldName == "end-margin")
    {
        setEndMargin(endMargin + increment);
    }
    else if (focusedFieldName == "create-new-program")
    {
        setCreateNewProgram(increment > 0);
    }
}

static std::shared_ptr<Sound>
createZone(const std::shared_ptr<Sampler> &sampler,
           const std::shared_ptr<Sound> &source, const int start, const int end,
           const int endMargin)
{
    const auto overlapInFrames =
        static_cast<int>(endMargin * source->getSampleRate() * 0.001);

    auto zone = sampler->addSound(source->getSampleRate());

    if (!zone)
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

        const auto frameData =
            source->isMono()
                ? std::vector{(*sourceSampleData)[frameIndex]}
                : std::vector{(*sourceSampleData)[frameIndex],
                              (*sourceSampleData)[frameIndex +
                                                  source->getLastFrameIndex()]};

        zone->insertFrame(frameData, zone->getLastFrameIndex() + 1);
    }

    zone->setEnd(zone->getFrameCount());

    return zone;
}

void EditSoundScreen::function(const int j)
{
    ScreenComponent::function(j);
    if (j != 4)
    {
        return;
    }

    if (const auto sound = sampler.lock()->getSound(); !sound)
    {
        return;
    }

    switch (edit)
    {
        case 0:
            handleDiscardEdit();
            break;
        case 1:
            handleLoopFromStartToEnd();
            break;
        case 2:
            handleSectionToNewSound();
            break;
        case 3:
            handleInsertSoundSectionStart();
            break;
        case 4:
            handleDeleteSection();
            break;
        case 5:
            handleSilenceSection();
            break;
        case 6:
            handleReverseSection();
            break;
        case 7:
            handleTimeStretch();
            break;
        case 8:
            handleNormalizeSection();
            break;
        case 9:
            handleSliceSound();
            break;
        default:;
    }

    ls.lock()->openScreen(returnToScreenName);
}

void EditSoundScreen::handleDiscardEdit() const
{
    const auto sound = sampler.lock()->getSound();
    auto [start, end] = getStartEndFromContext();
    const auto newLoopTo = sound->getLoopTo() - start;

    sampler.lock()->trimSample(sampler.lock()->getSoundIndex(), start, end);

    sound->setStart(0);
    sound->setEnd(sound->getEnd() - sound->getStart());
    sound->setMono(sound->isMono());
    sound->setLoopTo(newLoopTo);

    mpc.screens->get<ScreenId::ZoneScreen>()->initZones();
}

void EditSoundScreen::handleLoopFromStartToEnd() const
{
    const auto sound = sampler.lock()->getSound();
    sound->setLoopTo(sound->getStart());
}

void EditSoundScreen::handleSectionToNewSound() const
{
    const auto sound = sampler.lock()->getSound();
    const auto newSample = sampler.lock()->addSound();
    if (!newSample)
    {
        return;
    }

    newSample->setSampleRate(sound->getSampleRate());
    newSample->setName(newName);

    const auto newData = newSample->getMutableSampleData();
    newData->resize(sound->getSampleData()->size());
    for (int i = 0; i < newData->size(); i++)
    {
        (*newData)[i] = (*sound->getSampleData())[i];
    }

    newSample->setMono(sound->isMono());
    sampler.lock()->trimSample(sampler.lock()->getSoundCount() - 1,
                               sound->getStart(), sound->getEnd());
    sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
}

void EditSoundScreen::handleInsertSoundSectionStart() const
{
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    const auto source =
        sampler.lock()->getSortedSounds()[insertSoundIndex].first;
    const auto destination = sampler.lock()->getSound();

    const int destinationStartFrame = getStartEndFromContext().first;

    const auto srcFrames = source->getFrameCount();
    const auto dstFrames = destination->getFrameCount();
    const auto newFrames = srcFrames + dstFrames;
    auto newSamples = newFrames * (destination->isMono() ? 1 : 2);

    const auto newData = std::make_shared<std::vector<float>>(newSamples);
    const auto srcData = source->getSampleData();
    const auto dstData = destination->getSampleData();

    int srcCounter = 0, dstCounter = 0;
    for (int i = 0; i < newFrames; i++)
    {
        if (i < destinationStartFrame)
        {
            (*newData)[i] = (*dstData)[dstCounter];
            if (!destination->isMono())
            {
                (*newData)[i + newFrames] = (*dstData)[dstCounter + dstFrames];
            }
            dstCounter++;
        }
        else if (i < destinationStartFrame + srcFrames)
        {
            (*newData)[i] = (*srcData)[srcCounter];
            if (!destination->isMono())
            {
                (*newData)[i + newFrames] =
                    source->isMono() ? (*newData)[i]
                                     : (*srcData)[srcCounter + srcFrames];
            }
            srcCounter++;
        }
        else
        {
            (*newData)[i] = (*dstData)[dstCounter];
            if (!destination->isMono())
            {
                (*newData)[i + newFrames] = (*dstData)[dstCounter + dstFrames];
            }
            dstCounter++;
        }
    }

    destination->setSampleData(newData);
    destination->setEnd(newFrames);
    destination->setStart(0);
    destination->setMono(destination->isMono());
    destination->setLoopTo(destination->getEnd());
    zoneScreen->initZones();
}

void EditSoundScreen::handleDeleteSection() const
{
    const auto sound = sampler.lock()->getSound();
    auto [start, end] = getStartEndFromContext();
    sampler.lock()->deleteSection(sampler.lock()->getSoundIndex(), start, end);

    sound->setStart(0);
    sound->setEnd(sound->getFrameCount());
    sound->setLoopTo(sound->getFrameCount());

    mpc.screens->get<ScreenId::ZoneScreen>()->initZones();
}

void EditSoundScreen::handleSilenceSection() const
{
    const auto sound = sampler.lock()->getSound();
    auto [start, end] = getStartEndFromContext();
    auto newData = *sound->getSampleData();

    for (int i = start; i < end; i++)
    {
        newData[i] = 0.0f;
        if (!sound->isMono())
        {
            newData[i + sound->getFrameCount()] = 0.0f;
        }
    }

    sound->setSampleData(std::make_shared<std::vector<float>>(newData));
}

void EditSoundScreen::handleReverseSection() const
{
    const auto sound = sampler.lock()->getSound();
    auto [start, end] = getStartEndFromContext();
    const auto sampleData = sound->getSampleData();
    auto reverseCounter = end - 1;
    std::vector<float> newData(sampleData->size());

    if (sound->isMono())
    {
        for (int i = start; i < end; i++)
        {
            newData[i] = (*sampleData)[reverseCounter--];
        }
    }
    else
    {
        auto [left, right] = splitStereo(*sampleData);
        for (int i = start; i < end; i++)
        {
            left[i] = (*sampleData)[reverseCounter];
            right[i] = (*sampleData)[reverseCounter + sound->getFrameCount()];
            reverseCounter--;
        }
        newData = Sampler::mergeToStereo(left, right);
    }

    sound->setSampleData(std::make_shared<std::vector<float>>(newData));
}

void EditSoundScreen::handleTimeStretch() const
{
    if (timeStretchRatio == 10000)
    {
        return;
    }

    const float ratio = timeStretchRatio * 0.0001f;

    if (const auto sound = sampler.lock()->getSound(); sound->isMono())
    {
        const auto newSample = sampler.lock()->addSound(sound->getSampleRate());
        if (!newSample)
        {
            return;
        }

        TimeStretch ts(*sound->getSampleData(), ratio, sound->getSampleRate(),
                       timeStretchAdjust);
        newSample->setSampleData(
            std::make_shared<std::vector<float>>(ts.getProcessedData()));
        newSample->setMono(true);
        newSample->setName(newName);
    }
    else
    {
        const auto newSound = sampler.lock()->addSound(sound->getSampleRate());
        if (!newSound)
        {
            return;
        }

        auto [left, right] = splitStereo(*sound->getSampleData());
        TimeStretch tsL(left, ratio, sound->getSampleRate(), timeStretchAdjust);
        TimeStretch tsR(right, ratio, sound->getSampleRate(),
                        timeStretchAdjust);

        auto merged = Sampler::mergeToStereo(tsL.getProcessedData(),
                                             tsR.getProcessedData());
        newSound->setSampleData(std::make_shared<std::vector<float>>(merged));
        newSound->setMono(false);
        newSound->setName(newName);
    }
}

void EditSoundScreen::handleNormalizeSection() const
{
    const auto sound = sampler.lock()->getSound();
    auto [start, end] = getStartEndFromContext();
    auto data = *sound->getSampleData();

    float peak = 0.0f;
    for (int i = start; i < end; i++)
    {
        peak = std::max(peak, std::fabs(data[i]));
        if (!sound->isMono())
        {
            peak = std::max(peak, std::fabs(data[i + sound->getFrameCount()]));
        }
    }

    if (peak == 0.0f)
    {
        return;
    }
    const float factor = 1.0f / std::min(1.0f, peak);

    for (int i = start; i < end; i++)
    {
        data[i] *= factor;
        if (!sound->isMono())
        {
            data[i + sound->getFrameCount()] *= factor;
        }
    }

    sound->setSampleData(std::make_shared<std::vector<float>>(data));
}

void EditSoundScreen::handleSliceSound() const
{
    const auto source = sampler.lock()->getSound();
    const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
    const int zoneCount = zoneScreen->getZoneCount();

    for (int i = 0; i < zoneCount; i++)
    {
        const auto start = zoneScreen->getZoneStart(i);
        const auto end = zoneScreen->getZoneEnd(i);
        const auto zone =
            createZone(sampler.lock(), source, start, end, endMargin);
        if (!zone)
        {
            return;
        }

        zone->setName(i == 0 ? newName
                             : sampler.lock()->addOrIncreaseNumber(newName));
    }

    if (!createNewProgram)
    {
        return;
    }

    const auto p =
        sampler.lock()->createNewProgramAddFirstAvailableSlot().lock();
    p->setName(source->getName());

    for (int i = 0; i < zoneCount; i++)
    {
        const auto pad = p->getPad(i);
        const auto noteParams = p->getNoteParameters(pad->getNote());
        noteParams->setSoundIndex(sampler.lock()->getSoundCount() - zoneCount +
                                  i);
    }

    if (const auto drumBus = sequencer.lock()->getBus<DrumBus>(
            sequencer.lock()->getSelectedTrack()->getBusType()))
    {
        drumBus->setProgramIndex(
            ProgramIndex(sampler.lock()->getProgramCount() - 1));
    }
}

std::pair<int, int> EditSoundScreen::getStartEndFromContext() const
{
    const auto sound = sampler.lock()->getSound();
    int start = sound->getStart();
    int end = sound->getEnd();

    if (returnToScreenName == "loop")
    {
        start = sound->getLoopTo();
    }
    else if (returnToScreenName == "zone")
    {
        const auto zoneScreen = mpc.screens->get<ScreenId::ZoneScreen>();
        const auto zone = zoneScreen->getSelectedZoneIndex();
        start = zoneScreen->getZoneStart(zone);
        end = zoneScreen->getZoneEnd(zone);
    }

    return {start, end};
}

std::pair<std::vector<float>, std::vector<float>>
EditSoundScreen::splitStereo(const std::vector<float> &data) const
{
    const int half = data.size() / 2;
    return {std::vector(data.begin(), data.begin() + half),
            std::vector(data.begin() + half, data.end())};
}
