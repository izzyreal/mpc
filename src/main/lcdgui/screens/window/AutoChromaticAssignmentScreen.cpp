#include "AutoChromaticAssignmentScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sampler;
using namespace moduru::lang;
using namespace std;

AutoChromaticAssignmentScreen::AutoChromaticAssignmentScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "auto-chromatic-assignment", layerIndex)
{
}

void AutoChromaticAssignmentScreen::open()
{
    if (ls.lock()->getPreviousScreenName().compare("name") != 0)
    {
        auto letterNumber = sampler.lock()->getProgramCount() + 21;
        newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
        originalKey = 67;
        tune = 0;
    }
    
    init();
    
    setSourceSoundIndex(sampler.lock()->getLastNp(program.lock().get())->getSoundIndex());
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
            auto newProgram = sampler.lock()->addProgram().lock();
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
            
            auto programs = sampler.lock()->getPrograms();
            
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
    
    if (param.compare("source") == 0)
    {
        mpc.setNote(mpc.getNote() + i);
        displaySource();
        setSourceSoundIndex(sampler.lock()->getLastNp(program.lock().get())->getSoundIndex());
    }
    else if (param.compare("program-name") == 0)
    {
        auto nameScreen = mpc.screens->get<NameScreen>("name");
        auto autoChromaticAssignmentScreen = this;
        
        auto renamer = [autoChromaticAssignmentScreen](string& newName1) {
            autoChromaticAssignmentScreen->newName = newName1;
        };
        
        nameScreen->setName(newName);
        nameScreen->setRenamerAndScreenToReturnTo(renamer, "auto-chromatic-assignment");
        
        openScreen("name");
    }
    else if (param.compare("snd") == 0)
    {
        setSourceSoundIndex(sourceSoundIndex + i);
    }
    else if (param.compare("original-key") == 0)
    {
        setOriginalKey(originalKey + i);
    }
    else if (param.compare("tune") == 0)
    {
        setTune(tune + i);
    }
}

void AutoChromaticAssignmentScreen::setSourceSoundIndex(int i)
{
    if (i < -1 || i >= sampler.lock()->getSoundCount())
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
    auto note = sampler.lock()->getLastNp(program.lock().get())->getNumber();
    auto padIndex = program.lock()->getPadIndexFromNote(note);
    auto padName = sampler.lock()->getPadName(padIndex);
    findField("source").lock()->setText(to_string(note) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displayTune()
{
    string prefix = tune < 0 ? "-" : " ";
    findField("tune").lock()->setText(prefix + StrUtil::padLeft(to_string(abs(tune)), " ", 3));
}

void AutoChromaticAssignmentScreen::displayOriginalKey()
{
    auto padName = sampler.lock()->getPadName(originalKey - 35);
    findField("original-key").lock()->setText(to_string(originalKey) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displaySnd()
{
    auto sampleName = sourceSoundIndex == -1 ? "OFF" : sampler.lock()->getSoundName(sourceSoundIndex);
    string stereo = sourceSoundIndex == -1 ? "" : sampler.lock()->getSound(sourceSoundIndex).lock()->isMono() ? "" : "(ST)";
    findField("snd").lock()->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
}
void AutoChromaticAssignmentScreen::displayProgramName()
{
    findField("program-name").lock()->setText(newName);
}

void AutoChromaticAssignmentScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
    auto msg = nonstd::any_cast<string>(message);
    
    if (msg.compare("note") == 0)
    {
        displaySource();
        setSourceSoundIndex(sampler.lock()->getLastNp(program.lock().get())->getSoundIndex());
    }
}
