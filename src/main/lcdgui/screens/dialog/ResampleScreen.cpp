#include "ResampleScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

ResampleScreen::ResampleScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "resample", layerIndex)
{
}

void ResampleScreen::open()
{
    if (ls->isPreviousScreenNot({ScreenId::NameScreen}) && sampler->getSound())
    {
        newName = sampler->getSound()->getName();
        newName = sampler->addOrIncreaseNumber(newName);
        setNewFs(sampler->getSound()->getSampleRate());
    }

    displayNewBit();
    displayNewFs();
    displayNewName();
    displayQuality();
}

void ResampleScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "newfs")
    {
        setNewFs(newFs + i);
    }
    else if (focusedFieldName == "newbit")
    {
        setNewBit(newBit + i);
    }
    else if (focusedFieldName == "quality")
    {
        setQuality(quality + i);
    }
}

void ResampleScreen::openNameScreen()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "newname")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            setNewName(nameScreenName);
            openScreenById(ScreenId::ResampleScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(findField("newname")->getText(), 16, enterAction,
                               "resample");
        openScreenById(ScreenId::NameScreen);
    }
}

void ResampleScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SoundScreen);
            break;
        case 4:
        {
            const auto snd = sampler->getSound(sampler->getSoundIndex());
            const auto destSnd = sampler->addSound();

            if (destSnd == nullptr)
            {
                return;
            }

            destSnd->setName(newName);
            destSnd->setSampleRate(newFs);
            destSnd->setMono(snd->isMono());

            const auto source = snd->getSampleData();

            if (newFs != snd->getSampleRate())
            {
                sampler::Sampler::resample(source, snd->getSampleRate(),
                                           destSnd);
            }
            else
            {
                destSnd->setSampleData(
                    std::make_shared<std::vector<float>>(*source));
            }

            for (auto &f : *destSnd->getMutableSampleData())
            {
                if (f > 1)
                {
                    f = 1;
                }
                else if (f < -1)
                {
                    f = -1;
                }
            }

            destSnd->setName(newName);
            const int diff = newFs - snd->getSampleRate();
            int newTuning = static_cast<int>(diff * (120.f / newFs));
            if (newTuning < -120)
            {
                newTuning = -120;
            }
            else if (newTuning > 120)
            {
                newTuning = 120;
            }
            destSnd->setTune(newTuning);

            if (newBit == 1)
            {
                sampler::Sampler::process12Bit(
                    *destSnd->getMutableSampleData());
            }
            else if (newBit == 2)
            {
                sampler::Sampler::process8Bit(*destSnd->getMutableSampleData());
            }

            const auto ratio = newFs / (float)snd->getSampleRate();

            destSnd->setStart(snd->getStart() * ratio);
            destSnd->setEnd(snd->getEnd() * ratio);
            destSnd->setLoopTo(snd->getLoopTo() * ratio);

            sampler->setSoundIndex(sampler->getSoundCount() - 1);
            openScreenById(ScreenId::SoundScreen);
            break;
        }
    }
}

void ResampleScreen::displayNewFs()
{
    findField("newfs")->setText(std::to_string(newFs));
}

void ResampleScreen::displayQuality()
{
    findField("quality")->setText(qualityNames[quality]);
}

void ResampleScreen::displayNewBit()
{
    findField("newbit")->setText(bitNames[newBit]);
}

void ResampleScreen::displayNewName()
{
    findField("newname")->setText(newName);
}

void ResampleScreen::setNewFs(const int i)
{
    newFs = std::clamp(i, 4000, 65000);
    displayNewFs();
}

void ResampleScreen::setQuality(const int i)
{
    quality = std::clamp(i, 0, 2);
    displayQuality();
}

void ResampleScreen::setNewBit(const int i)
{
    newBit = std::clamp(i, 0, 2);
    displayNewBit();
}

void ResampleScreen::setNewName(const std::string &s)
{
    newName = s;
    displayNewName();
}
