#include <controls/other/AbstractOtherControls.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>
#include <ui/NameGui.hpp>
#include <ui/UserDefaults.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::other;
using namespace std;

AbstractOtherControls::AbstractOtherControls() 
	: BaseControls()
{
	akaiAsciiChar = mpc::Mpc::akaiAsciiChar;
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
}

void AbstractOtherControls::init()
{
    super::init();
	tickValues = Mpc::instance().getSequencer().lock()->getTickValues();
	auto lSequencer = sequencer.lock();
    seqNum = lSequencer->getActiveSequenceIndex();
    sequence = lSequencer->getSequence(seqNum);
    trackNum = lSequencer->getActiveTrackIndex();
    track = sequence.lock()->getTrack(trackNum);
	auto lSampler = sampler.lock();
	auto lTrk = track.lock();
    program = lTrk->getBusNumber() == 0 ? weak_ptr<mpc::sampler::Program>() : dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(lSampler->getDrumBusProgramNumber(lTrk->getBusNumber())).lock());
}

AbstractOtherControls::~AbstractOtherControls() {
}
