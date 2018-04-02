#include <controls/misc/AbstractMiscControls.hpp>

#include <ui/misc/PunchGui.hpp>
#include <ui/misc/SecondSeqGui.hpp>
#include <ui/misc/TransGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::misc;
using namespace std;

AbstractMiscControls::AbstractMiscControls(mpc::Mpc* mpc) 
	: AbstractControls(mpc)
{
	punchGui = mpc->getUis().lock()->getPunchGui();
	transGui = mpc->getUis().lock()->getTransGui();
	secondSeqGui = mpc->getUis().lock()->getSecondSeqGui();
}


/*
void AbstractMiscControls::checkAllTimes(int notch)
{
    init();
    auto sequence = npc(sequencer)->getActiveSequence();
    if(param.compare("time0") == 0)) {
        punchGui->setTime0(mpc::sequencer::SeqUtil::getTickFromBar((mpc::sequencer::SeqUtil::getBarFromTick(sequence, punchGui->getTime0())) + i, sequence, punchGui->getTime0()));
    }
    if(param.compare("time1") == 0)) {
        punchGui->setTime0(mpc::sequencer::SeqUtil::setBeat((mpc::sequencer::SeqUtil::getBeat(sequence, punchGui->getTime0())) + i, sequence, punchGui->getTime0()));
    }
    if(param.compare("time2") == 0)) {
        punchGui->setTime0(mpc::sequencer::SeqUtil::setClockNumber((mpc::sequencer::SeqUtil::getClockNumber(sequence, punchGui->getTime0())) + i, sequence, punchGui->getTime0()));
    }
    if(param.compare("time3") == 0)) {
        punchGui->setTime1(mpc::sequencer::SeqUtil::getTickFromBar((mpc::sequencer::SeqUtil::getBarFromTick(sequence, punchGui->getTime1())) + i, sequence, punchGui->getTime1()));
    }
    if(param.compare("time4") == 0)) {
        punchGui->setTime1(mpc::sequencer::SeqUtil::setBeat((mpc::sequencer::SeqUtil::getBeat(sequence, punchGui->getTime1())) + i, sequence, punchGui->getTime1()));
    }
    if(param.compare("time5") == 0)) {
        punchGui->setTime1(mpc::sequencer::SeqUtil::setClockNumber((mpc::sequencer::SeqUtil::getClockNumber(sequence, punchGui->getTime1())) + i, sequence, punchGui->getTime1()));
    }
}
*/

AbstractMiscControls::~AbstractMiscControls() {
}
