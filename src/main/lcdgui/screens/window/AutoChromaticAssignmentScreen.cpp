#include "AutoChromaticAssignmentScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Pad.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;

AutoChromaticAssignmentScreen::AutoChromaticAssignmentScreen(
    Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "auto-chromatic-assignment", layerIndex)
{
}

void AutoChromaticAssignmentScreen::open()
{
    if (ls->isPreviousScreenNot({ScreenId::NameScreen}))
    {
        const auto letterNumber = sampler->getProgramCount() + 21;
        newName = "NewPgm-" + Mpc::akaiAscii[letterNumber];
        originalKey = DrumNoteNumber(67);
        tune = 0;
    }

    const auto program = getProgramOrThrow();
    const auto selectedNoteParameters = program->getNoteParameters(
        mpc.clientEventController->getSelectedNote());
    setSourceSoundIndex(selectedNoteParameters->getSoundIndex());
    displayOriginalKey();
    displayTune();
    displayProgramName();
    displaySource();

    mpc.clientEventController->addObserver(this);
}

void AutoChromaticAssignmentScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void AutoChromaticAssignmentScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        case 4:
        {
            const auto newProgram =
                sampler->createNewProgramAddFirstAvailableSlot().lock();
            newProgram->setName(newName);

            for (int j = MinDrumNoteNumber; j <= MaxDrumNoteNumber; j++)
            {
                const auto pad = newProgram->getPad(j - MinDrumNoteNumber);
                pad->setNote(DrumNoteNumber(j));
                auto noteParameters = newProgram->getNoteParameters(j);
                //                const auto noteParameters =
                //                    new NoteParameters(j - MinDrumNoteNumber);
                //                newProgram->setNoteParameters(j -
                //                MinDrumNoteNumber,
                //                                              noteParameters);
                noteParameters->setSoundIndex(sourceSoundIndex);

                noteParameters->setTune((j - originalKey) * 10 + tune);
            }

            const auto programs = sampler->getPrograms();

            for (int j = MinProgramIndex; j <= MaxProgramIndex; j++)
            {
                if (programs[j].lock() == newProgram)
                {
                    getActiveDrumBus()->setProgramIndex(ProgramIndex(j));
                    break;
                }
            }

            openScreenById(ScreenId::PgmAssignScreen);
            break;
        }
        default:;
    }
}

void AutoChromaticAssignmentScreen::turnWheel(const int i)
{

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "source")
    {
        const auto program = getProgramOrThrow();
        mpc.clientEventController->setSelectedNote(
            mpc.clientEventController->getSelectedNote() + i);
        displaySource();
        const auto selectedNoteParameters = program->getNoteParameters(
            mpc.clientEventController->getSelectedNote());
        setSourceSoundIndex(selectedNoteParameters->getSoundIndex());
    }
    else if (focusedFieldName == "snd")
    {
        setSourceSoundIndex(sourceSoundIndex + i);
    }
    else if (focusedFieldName == "original-key")
    {
        setOriginalKey(originalKey + i);
    }
    else if (focusedFieldName == "tune")
    {
        setTune(tune + i);
    }
}

void AutoChromaticAssignmentScreen::openNameScreen()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "program-name")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            newName = nameScreenName;
            openScreenById(ScreenId::AutoChromaticAssignmentScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(newName, 16, enterAction,
                               "auto-chromatic-assignment");
        openScreenById(ScreenId::NameScreen);
    }
}

void AutoChromaticAssignmentScreen::setSourceSoundIndex(const int i)
{
    sourceSoundIndex =
        std::clamp(i, -1, std::max(0, sampler->getSoundCount() - 1));
    displaySnd();
}

void AutoChromaticAssignmentScreen::setOriginalKey(
    const DrumNoteNumber drumNoteNumber)
{
    originalKey =
        std::clamp(drumNoteNumber, MinDrumNoteNumber, MaxDrumNoteNumber);
    displayOriginalKey();
}

void AutoChromaticAssignmentScreen::setTune(const int i)
{
    tune = std::clamp(i, -240, 240);
    displayTune();
}

void AutoChromaticAssignmentScreen::displaySource() const
{
    const auto program = getProgramOrThrow();
    const auto note = mpc.clientEventController->getSelectedNote();
    const auto padIndex = program->getPadIndexFromNote(note);
    const auto padName = sampler->getPadName(padIndex);
    findField("source")->setText(std::to_string(note) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displayTune() const
{
    const std::string prefix = tune < 0 ? "-" : " ";
    findField("tune")->setText(
        prefix + StrUtil::padLeft(std::to_string(abs(tune)), " ", 3));
}

void AutoChromaticAssignmentScreen::displayOriginalKey() const
{
    const auto padName = sampler->getPadName(originalKey - 35);
    findField("original-key")
        ->setText(std::to_string(originalKey) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displaySnd() const
{
    const auto sampleName = sourceSoundIndex == -1
                                ? "OFF"
                                : sampler->getSoundName(sourceSoundIndex);
    const std::string stereo = sourceSoundIndex == -1 ? ""
                               : sampler->getSound(sourceSoundIndex)->isMono()
                                   ? ""
                                   : "(ST)";
    findField("snd")->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
}
void AutoChromaticAssignmentScreen::displayProgramName() const
{
    findField("program-name")->setText(newName);
}

void AutoChromaticAssignmentScreen::update(Observable *observable,
                                           const Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        displaySource();
        const auto program = getProgramOrThrow();
        const auto selectedNoteParameters = program->getNoteParameters(
            mpc.clientEventController->getSelectedNote());
        setSourceSoundIndex(selectedNoteParameters->getSoundIndex());
    }
}
