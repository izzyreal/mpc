#include <controls/other/AbstractOtherControls.hpp>

#include <Mpc.hpp>
#include <StartUp.hpp>
#include <ui/NameGui.hpp>
#include <ui/UserDefaults.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::other;
using namespace std;

AbstractOtherControls::AbstractOtherControls(mpc::Mpc* mpc) 
	: AbstractControls(mpc)
{
	akaiAsciiChar = mpc::Mpc::akaiAsciiChar;
	nameGui = mpc->getUis().lock()->getNameGui();
}

void AbstractOtherControls::init()
{
    super::init();
	tickValues = mpc->getSequencer().lock()->getTickValues();
	auto lSequencer = sequencer.lock();
    seqNum = lSequencer->getActiveSequenceIndex();
    sequence = lSequencer->getSequence(seqNum);
    trackNum = lSequencer->getActiveTrackIndex();
    track = sequence.lock()->getTrack(trackNum);
	auto lSampler = sampler.lock();
	auto lTrk = track.lock();
    program = lTrk->getBusNumber() == 0 ? weak_ptr<mpc::sampler::Program>() : lSampler->getProgram(lSampler->getDrumBusProgramNumber(lTrk->getBusNumber()));
}

AbstractOtherControls::~AbstractOtherControls() {
}
