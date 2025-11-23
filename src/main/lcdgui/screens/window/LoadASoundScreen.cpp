#include "LoadASoundScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Track.hpp"

#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "lcdgui/Label.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens;

LoadASoundScreen::LoadASoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-sound", layerIndex)
{
}

void LoadASoundScreen::open()
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    findLabel("filename")
        ->setText("File:" +
                  loadScreen->getSelectedFile()->getNameWithoutExtension());
    assignToNote = mpc.clientEventController->getSelectedNote();
    displayAssignToNote();
    mpc.clientEventController->addObserver(
        this); // Subscribe to "note" messages
}

void LoadASoundScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void LoadASoundScreen::turnWheel(const int i)
{

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "assign-to-note")
    {
        const auto newAssignToNote =
            std::clamp(assignToNote + i, NoDrumNoteAssigned, MaxDrumNoteNumber);

        if (newAssignToNote == NoDrumNoteAssigned)
        {
            mpc.clientEventController->setSelectedNote(MinDrumNoteNumber);
            assignToNote = newAssignToNote;
            displayAssignToNote();
        }
        else
        {
            mpc.clientEventController->setSelectedNote(newAssignToNote);
        }
    }
}

void LoadASoundScreen::function(const int i)
{
    switch (i)
    {
        case 2:
        {
            const auto s = sampler.lock()->getPreviewSound();
            const auto start = s->getStart();
            const auto end = s->getLastFrameIndex();
            auto loopTo = -1;

            if (s->isLoopEnabled())
            {
                loopTo = s->getLoopTo();
            }

            sampler.lock()->playPreviewSample(start, end, loopTo);
            break;
        }
        case 3:

            mpc.getEngineHost()
                ->getSequencerPlaybackEngine()
                ->enqueueEventAfterNFrames(
                    [&]
                    {
                        mpc.getEngineHost()
                            ->getPreviewSoundPlayer()
                            ->finishVoice();
                        sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
                    },
                    0);

            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
            keepSound();
            break;
        default:;
    }
}

void LoadASoundScreen::keepSound() const
{
    auto previewSound = sampler.lock()->getPreviewSound();
    const auto candidateSoundName = previewSound->getName();

    int existingSoundIndex = -1;

    for (int i = 0; i < sampler.lock()->getSoundCount(); i++)
    {
        auto s = sampler.lock()->getSound(i);
        if (s == previewSound)
        {
            continue;
        }

        if (StrUtil::eqIgnoreCase(s->getName(), candidateSoundName))
        {
            existingSoundIndex = i;
            break;
        }
    }

    auto actionAfterLoadingSound = [this](bool)
    {
        const auto soundIndex = sampler.lock()->getSoundCount() - 1;

        if (assignToNote != NoDrumNoteAssigned)
        {
            getProgramOrThrow()
                ->getNoteParameters(assignToNote)
                ->setSoundIndex(soundIndex);
        }

        sampler.lock()->setSoundIndex(soundIndex);
    };

    if (existingSoundIndex >= 0)
    {
        auto replaceAction = [this, existingSoundIndex, actionAfterLoadingSound]
        {
            const auto previewSound = sampler.lock()->getPreviewSound();
            const auto isMono = previewSound->isMono();
            sampler.lock()->replaceSound(existingSoundIndex, previewSound);
            actionAfterLoadingSound(isMono);
            openScreenById(ScreenId::LoadScreen);
        };

        const auto initializeNameScreen =
            [this, actionAfterLoadingSound, previewSound]
        {
            const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();

            auto enterAction = [this, actionAfterLoadingSound,
                                previewSound](const std::string &nameScreenName)
            {
                if (sampler.lock()->checkExists(nameScreenName) >= 0)
                {
                    return;
                }

                previewSound->setName(nameScreenName);
                actionAfterLoadingSound(previewSound->isMono());
                openScreenById(ScreenId::LoadScreen);
            };

            const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
            auto mainScreenAction = [&]
            {
                sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
            };
            nameScreen->initialize(
                loadScreen->getSelectedFile()->getNameWithoutExtension(), 16,
                enterAction, "load", mainScreenAction);
        };

        const auto fileExistsScreen =
            mpc.screens->get<ScreenId::FileExistsScreen>();
        fileExistsScreen->initialize(replaceAction, initializeNameScreen,
                                     [this]
                                     {
                                         sampler.lock()->deleteSound(
                                             sampler.lock()->getPreviewSound());
                                         openScreenById(ScreenId::LoadScreen);
                                     });
        openScreenById(ScreenId::FileExistsScreen);
        return;
    }

    actionAfterLoadingSound(previewSound->isMono());
    openScreenById(ScreenId::LoadScreen);
}

void LoadASoundScreen::displayAssignToNote() const
{
    const auto padIndex =
        getProgramOrThrow()->getPadIndexFromNote(assignToNote);
    const auto padName = sampler.lock()->getPadName(padIndex);
    const auto noteName =
        std::string(assignToNote == 34 ? "--" : std::to_string(assignToNote));
    findField("assign-to-note")->setText(noteName + "/" + padName);
}

void LoadASoundScreen::update(Observable *observable, const Message message)
{
    if (const auto msg = std::get<std::string>(message); msg == "note")
    {
        assignToNote = mpc.clientEventController->getSelectedNote();
        displayAssignToNote();
    }
}
