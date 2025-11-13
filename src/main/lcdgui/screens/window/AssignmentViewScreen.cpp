#include "AssignmentViewScreen.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Pad.hpp"

#include "StrUtil.hpp"

#include "lcdgui/Label.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

AssignmentViewScreen::AssignmentViewScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "assignment-view", layerIndex)
{
}

void AssignmentViewScreen::open()
{
    findField("note")->setFocusable(false);
    findField("note")->setInverted(true);

    ls->setFocus(getFocusFromPadIndex());

    displayAssignmentView();
    // Subscribe to "pad" and "bank" messages
    mpc.clientEventController->addObserver(this);
}

void AssignmentViewScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void AssignmentViewScreen::up()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("0") != std::string::npos)
    {
        return;
    }

    const auto padIndex = mpc.clientEventController->getSelectedPad() + 4;
    ls->setFocus(padFocusNames[padIndex % 16]);
    mpc.clientEventController->setSelectedPad(padIndex);
}

void AssignmentViewScreen::down()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("3") != std::string::npos)
    {
        return;
    }

    const auto padIndex = mpc.clientEventController->getSelectedPad() - 4;
    ls->setFocus(padFocusNames[padIndex % 16]);
    mpc.clientEventController->setSelectedPad(padIndex);
}

void AssignmentViewScreen::left()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("a") != std::string::npos)
    {
        return;
    }

    ScreenComponent::left();

    const auto padIndex = mpc.clientEventController->getSelectedPad() - 1;
    ls->setFocus(padFocusNames[padIndex % 16]);
    mpc.clientEventController->setSelectedPad(padIndex);
}

void AssignmentViewScreen::right()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("d") != std::string::npos)
    {
        return;
    }

    ScreenComponent::right();
    const auto padIndex = mpc.clientEventController->getSelectedPad() + 1;
    ls->setFocus(padFocusNames[padIndex % 16]);
    mpc.clientEventController->setSelectedPad(padIndex);
}

void AssignmentViewScreen::turnWheel(const int i)
{
    const auto program = getProgramOrThrow();
    const auto selectedPad =
        program->getPad(mpc.clientEventController->getSelectedPad());
    selectedPad->setNote(selectedPad->getNote() + i);
    displayNote();
    displaySoundName();
    displayPad(selectedPad->getIndex() % 16);
}

void AssignmentViewScreen::update(Observable *o, const Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "bank")
    {
        displayAssignmentView();
    }
    else if (msg == "pad")
    {
        ls->setFocus(getFocusFromPadIndex());
        displayAssignmentView();
    }
    else if (msg == "note")
    {
        displayNote();
        displaySoundName();
        displayPad(getPadIndexFromFocus());
    }
}

void AssignmentViewScreen::displayAssignmentView()
{
    for (int i = 0; i < 16; i++)
    {
        displayPad(i);
    }

    displayBankInfoAndNoteLabel();
    displayNote();
    displaySoundName();
}

void AssignmentViewScreen::displayPad(const int i) const
{
    const auto program = getProgramOrThrow();
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    const auto note = program->getPad(i + 16 * bank)->getNote();

    std::string sampleName;

    if (note != 34)
    {
        const auto sampleNumber =
            program->getNoteParameters(note)->getSoundIndex();
        sampleName =
            sampleNumber != -1 ? sampler->getSoundName(sampleNumber) : "--";

        if (sampleName.length() > 8)
        {
            sampleName = StrUtil::trim(sampleName.substr(0, 8));
        }
    }

    findField(padFocusNames[i])->setText(sampleName);
}

void AssignmentViewScreen::displayBankInfoAndNoteLabel() const
{
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    findLabel("info0")->setText("Bank:" + letters[bank] + " Note:");
}

void AssignmentViewScreen::displayNote()
{
    const auto program = getProgramOrThrow();
    const auto note = program->getPad(getPadIndexFromFocus())->getNote();
    const auto text = note == 34 ? "--" : std::to_string(note);
    findField("note")->setText(text);
}

void AssignmentViewScreen::displaySoundName()
{

    const auto padIndex = getPadIndexFromFocus();
    const auto program = getProgramOrThrow();
    const int note = program->getPad(padIndex)->getNote();

    if (note == 34)
    {
        findLabel("info2")->setText("=");
        return;
    }

    const int soundIndex = program->getNoteParameters(note)->getSoundIndex();

    const std::string soundName =
        soundIndex == -1 ? "OFF" : sampler->getSoundName(soundIndex);

    const std::string stereo =
        soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)"
                                                                     : "";

    findLabel("info2")->setText("=" + StrUtil::padRight(soundName, " ", 16) +
                                stereo);
}

int AssignmentViewScreen::getPadIndexFromFocus()
{
    int padIndex = -1;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    for (int i = 0; i < padFocusNames.size(); i++)
    {
        if (padFocusNames[i] == focusedFieldName)
        {
            padIndex = i;
            break;
        }
    }

    return padIndex;
}

std::string AssignmentViewScreen::getFocusFromPadIndex() const
{
    auto padIndex = mpc.clientEventController->getSelectedPad();

    while (padIndex > 15)
    {
        padIndex -= 16;
    }

    return padFocusNames[padIndex];
}
