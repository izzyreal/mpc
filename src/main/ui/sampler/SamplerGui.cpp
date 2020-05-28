#include "SamplerGui.hpp"

#include <Mpc.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::sampler;
using namespace std;

string SamplerGui::getPrevScreenName()
{
    return prevScreenName;
}

void SamplerGui::setPrevScreenName(string s)
{
    prevScreenName = s;
}

void SamplerGui::setNewName(string newName)
{
    this->newName = newName;
}

string SamplerGui::getNewName()
{
    return newName;
}
