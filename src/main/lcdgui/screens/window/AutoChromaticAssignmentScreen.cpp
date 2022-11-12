#include "AutoChromaticAssignmentScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sampler;
using namespace moduru::lang;

AutoChromaticAssignmentScreen::AutoChromaticAssignmentScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "auto-chromatic-assignment", layerIndex)
{
}

void AutoChromaticAssignmentScreen::open()
{
    if (ls.lock()->getPreviousScreenName() != "name")
    {
        auto letterNumber = sampler->getProgramCount() + 21;
        newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
        originalKey = 67;
        tune = 0;
    }
    
    init();
    
    setSourceSoundIndex(sampler->getLastNp(program.lock().get())->getSoundIndex());
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
            openScreen("program-assign");
            break;
        case 4:
        {
            auto newProgram = sampler->addProgram().lock();
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
                    mpcSoundPlayerChannel->setProgram(j);
                    break;
                }
            }
            
            openScreen("program-assign");
            break;
        }
    }
}

void AutoChromaticAssignmentScreen::turnWheel(int i)
{
    init();
    
    if (param == "source")
    {
        mpc.setNote(mpc.getNote() + i);
        displaySource();
        setSourceSoundIndex(sampler->getLastNp(program.lock().get())->getSoundIndex());
    }
    else if (param == "program-name")
    {
        auto nameScreen = mpc.screens->get<NameScreen>("name");
        auto autoChromaticAssignmentScreen = this;
        
        auto renamer = [autoChromaticAssignmentScreen](std::string& newName1) {
            autoChromaticAssignmentScreen->newName = newName1;
        };
        
        nameScreen->setName(newName);
        nameScreen->setRenamerAndScreenToReturnTo(renamer, "auto-chromatic-assignment");
        
        openScreen("name");
    }
    else if (param == "snd")
    {
        setSourceSoundIndex(sourceSoundIndex + i);
    }
    else if (param == "original-key")
    {
        setOriginalKey(originalKey + i);
    }
    else if (param == "tune")
    {
        setTune(tune + i);
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
    auto note = sampler->getLastNp(program.lock().get())->getNumber();
    auto padIndex = program.lock()->getPadIndexFromNote(note);
    auto padName = sampler->getPadName(padIndex);
    findField("source").lock()->setText(std::to_string(note) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displayTune()
{
    std::string prefix = tune < 0 ? "-" : " ";
    findField("tune").lock()->setText(prefix + StrUtil::padLeft(std::to_string(abs(tune)), " ", 3));
}

void AutoChromaticAssignmentScreen::displayOriginalKey()
{
    auto padName = sampler->getPadName(originalKey - 35);
    findField("original-key").lock()->setText(std::to_string(originalKey) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displaySnd()
{
    auto sampleName = sourceSoundIndex == -1 ? "OFF" : sampler->getSoundName(sourceSoundIndex);
    std::string stereo = sourceSoundIndex == -1 ? "" : sampler->getSound(sourceSoundIndex).lock()->isMono() ? "" : "(ST)";
    findField("snd").lock()->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
}
void AutoChromaticAssignmentScreen::displayProgramName()
{
    findField("program-name").lock()->setText(newName);
}

void AutoChromaticAssignmentScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
    auto msg = nonstd::any_cast<std::string>(message);
    
    if (msg == "note")
    {
        displaySource();
        setSourceSoundIndex(sampler->getLastNp(program.lock().get())->getSoundIndex());
    }
}
