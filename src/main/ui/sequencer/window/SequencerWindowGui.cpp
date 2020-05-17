#include "SequencerWindowGui.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/TempoChangeEvent.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

int SequencerWindowGui::getTapAvg()
{
    return tapAvg;
}

void SequencerWindowGui::setTapAvg(int i)
{
    tapAvg = i;
}

bool SequencerWindowGui::isPgmChangeToSeqEnabled()
{
    return pgmChangeToSeqEnabled;
}

void SequencerWindowGui::setPgmChangeToSeqEnabled(bool b)
{
    pgmChangeToSeqEnabled = b;
}
