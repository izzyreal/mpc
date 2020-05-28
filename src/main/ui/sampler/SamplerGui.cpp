#include "SamplerGui.hpp"

#include <Mpc.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::sampler;
using namespace std;

void SamplerGui::setNewName(string newName)
{
    this->newName = newName;
}

string SamplerGui::getNewName()
{
    return newName;
}
