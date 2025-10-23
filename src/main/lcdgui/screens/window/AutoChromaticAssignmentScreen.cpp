#include "AutoChromaticAssignmentScreen.hpp"
#include "sampler/Pad.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;

AutoChromaticAssignmentScreen::AutoChromaticAssignmentScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "auto-chromatic-assignment", layerIndex)
{
}

void AutoChromaticAssignmentScreen::open()
{
    if (ls->isPreviousScreenNot<NameScreen>())
    {
        auto letterNumber = sampler->getProgramCount() + 21;
        newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
        originalKey = 67;
        tune = 0;
    }
    
    init();
    
    setSourceSoundIndex(sampler->getLastNp(program.get())->getSoundIndex());
    displayOriginalKey();
    displayTune();
    displayProgramName();
    displaySource();
    
    mpc.addObserver(this);
}

void AutoChromaticAssignmentScreen::close()
{
    mpc.deleteObserver(this);
}

void AutoChromaticAssignmentScreen::function(int i)
{
    init();
    
    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        case 4:
        {
            auto newProgram = sampler->createNewProgramAddFirstAvailableSlot().lock();
            newProgram->setName(newName);
            
            for (int j = 35; j <= 98; j++)
            {
                auto pad = newProgram->getPad(j - 35);
                pad->setNote(j);
                auto noteParameters = new NoteParameters(j - 35);
                newProgram->setNoteParameters(j - 35, noteParameters);
                noteParameters->setSoundIndex(sourceSoundIndex);
                
                noteParameters->setTune(((j - originalKey) * 10) + tune);
            }
            
            auto programs = sampler->getPrograms();
            
            for (int j = 0; j < programs.size(); j++)
            {
                if (programs[j].lock() == newProgram)
                {
                    activeDrum().setProgram(j);
                    break;
                }
            }
            
            mpc.getLayeredScreen()->openScreen<PgmAssignScreen>();
            break;
        }
    }
}

void AutoChromaticAssignmentScreen::turnWheel(int i)
{
    init();
    
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "source")
    {
        mpc.setNote(mpc.getNote() + i);
        displaySource();
        setSourceSoundIndex(sampler->getLastNp(program.get())->getSoundIndex());
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
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "program-name")
    {
        const auto enterAction = [this](std::string& nameScreenName) {
            newName = nameScreenName;
            mpc.getLayeredScreen()->openScreen<AutoChromaticAssignmentScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        nameScreen->initialize(newName, 16, enterAction, "auto-chromatic-assignment");
        mpc.getLayeredScreen()->openScreen<NameScreen>();
    }
}

void AutoChromaticAssignmentScreen::setSourceSoundIndex(int i)
{
    if (i < -1 || i >= sampler->getSoundCount())
        return;
    
    sourceSoundIndex = i;
    displaySnd();
}

void AutoChromaticAssignmentScreen::setOriginalKey(int i)
{
    if (i < 35 || i > 98)
        return;
    
    originalKey = i;
    displayOriginalKey();
}

void AutoChromaticAssignmentScreen::setTune(int i)
{
    if (i < -240 || i > 240)
        return;
    
    tune = i;
    displayTune();
}

void AutoChromaticAssignmentScreen::displaySource()
{
    auto note = sampler->getLastNp(program.get())->getNumber();
    auto padIndex = program->getPadIndexFromNote(note);
    auto padName = sampler->getPadName(padIndex);
    findField("source")->setText(std::to_string(note) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displayTune()
{
    std::string prefix = tune < 0 ? "-" : " ";
    findField("tune")->setText(prefix + StrUtil::padLeft(std::to_string(abs(tune)), " ", 3));
}

void AutoChromaticAssignmentScreen::displayOriginalKey()
{
    auto padName = sampler->getPadName(originalKey - 35);
    findField("original-key")->setText(std::to_string(originalKey) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displaySnd()
{
    auto sampleName = sourceSoundIndex == -1 ? "OFF" : sampler->getSoundName(sourceSoundIndex);
    std::string stereo = sourceSoundIndex == -1 ? "" : sampler->getSound(sourceSoundIndex)->isMono() ? "" : "(ST)";
    findField("snd")->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
}
void AutoChromaticAssignmentScreen::displayProgramName()
{
    findField("program-name")->setText(newName);
}

void AutoChromaticAssignmentScreen::update(Observable* observable, Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        displaySource();
        setSourceSoundIndex(sampler->getLastNp(program.get())->getSoundIndex());
    }
}
