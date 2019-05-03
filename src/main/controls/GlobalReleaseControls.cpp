#include <controls/GlobalReleaseControls.hpp>

#include <controls/Controls.hpp>

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::controls;
using namespace std;

GlobalReleaseControls::GlobalReleaseControls(mpc::Mpc* mpc)
	: AbstractControls(mpc)
{
}

void GlobalReleaseControls::function(int i) {
	init();
	auto controls = mpc->getControls().lock();
	switch (i) {
	case 0:
		if (csn.compare("step_tc") == 0) {
			ls.lock()->openScreen("sequencer_step");
		}
		break;
	case 2:
		controls->setF3Pressed(false);
		if (csn.compare("loadasound") == 0) {
			sampler.lock()->finishBasicVoice();
		}
		break;
	case 3:
		controls->setF4Pressed(false);
		if (csn.compare("keeporretry") == 0) {
			sampler.lock()->finishBasicVoice();
		}
		break;
	case 4:
		controls->setF5Pressed(false);
	case 5:
		controls->setF6Pressed(false);
		if (!sequencer.lock()->isPlaying() && csn.compare("sequencer") != 0) {
			sampler.lock()->finishBasicVoice();
		}
		if (csn.compare("trackmute") == 0) {
			if (!sequencer.lock()->isSoloEnabled()) {
				ls.lock()->setCurrentBackground("trackmute");
			}
			sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
		}
		break;
	}
}

void GlobalReleaseControls::simplePad(int i)
{
	init();
	auto bank = samplerGui->getBank();
	auto controls = mpc->getControls().lock();
	if (controls->getPressedPads()->find(i) == controls->getPressedPads()->end()) return;
	controls->getPressedPads()->erase(controls->getPressedPads()->find(i));

	//if (csn.compare("loadasound") == 0) return;

	auto lTrk = track.lock();
	auto note = lTrk->getBusNumber() > 0 ? program.lock()->getPad(i + (bank * 16))->getNote() : i + (bank * 16) + 35;
	generateNoteOff(note);
	bool posIsLastTick = sequencer.lock()->getTickPosition() == sequencer.lock()->getActiveSequence().lock()->getLastTick();

	bool maybeRecWithoutPlaying = csn.compare("sequencer") == 0 && !posIsLastTick;
	bool stepRec = csn.compare("sequencer_step") == 0 && !posIsLastTick;
	if (stepRec || maybeRecWithoutPlaying) {
		auto newDur = static_cast<int>(mpc->getAudioMidiServices().lock()->getFrameSequencer().lock()->getTickPosition());
		sequencer.lock()->stopMetronomeTrack();
		bool adjustedRecordedNote = lTrk->adjustDurLastEvent(newDur);
		if (adjustedRecordedNote && maybeRecWithoutPlaying) {
			auto swGui = mpc->getUis().lock()->getSequencerWindowGui();
			int noteVal = swGui->getNoteValue();
			int stepLength = sequencer.lock()->getTickValues()[noteVal];
			int nextPos = sequencer.lock()->getTickPosition() + stepLength;
			//MLOG("\nnextpos1: " + to_string(nextPos));
			auto bar = sequencer.lock()->getCurrentBarNumber() + 1;
			nextPos = lTrk->timingCorrectTick(0, bar, nextPos, stepLength);
			//MLOG("nextpos2: " + to_string(nextPos));
			auto lastTick = sequencer.lock()->getActiveSequence().lock()->getLastTick();
			if (nextPos != 0 && nextPos < lastTick) {
				nextPos = lTrk->swingTick(nextPos, noteVal, swGui->getSwing());
				sequencer.lock()->move(nextPos);
			}
			else {
				sequencer.lock()->move(lastTick);
			}
		}
	}
}

void GlobalReleaseControls::generateNoteOff(int nn)
{
    init();
	auto lSequencer = sequencer.lock();
	auto lTrk = track.lock();
    if(lSequencer->isRecordingOrOverdubbing()) {
        auto n = new mpc::sequencer::NoteEvent();
        n->setNote(nn);
        n->setVelocity(0);
        n->setTick(lSequencer->getTickPosition());
        lTrk->recordNoteOff(n);
    }
    auto noteEvent = make_shared<mpc::sequencer::NoteEvent>(nn);
    noteEvent->setVelocity(0);
    noteEvent->setDuration(0);
    noteEvent->setTick(-1);
    mpc->getEventHandler().lock()->handle(noteEvent, lTrk.get());
}

void GlobalReleaseControls::overDub()
{
	auto controls = mpc->getControls().lock();
	controls->setOverDubPressed(false);
    init();
	auto hw = mpc->getHardware().lock();
	hw->getLed("overdub").lock()->light(sequencer.lock()->isOverDubbing());
}

void GlobalReleaseControls::rec()
{
	auto controls = mpc->getControls().lock();
	controls->setRecPressed(false);
    init();
	auto hw = mpc->getHardware().lock();
	hw->getLed("rec").lock()->light(sequencer.lock()->isRecording());
}

void GlobalReleaseControls::tap()
{
	auto controls = mpc->getControls().lock();
	controls->setTapPressed(false);
	auto lSequencer = sequencer.lock();
    if (lSequencer->isRecordingOrOverdubbing())
        lSequencer->flushTrackNoteCache();
}

void GlobalReleaseControls::shift()
{
	auto controls = mpc->getControls().lock();
	controls->setShiftPressed(false);
	init();
	if (csn.compare("sequencer_step") == 0 && param.length() == 2) {
		auto eventNumber = stoi(param.substr(1, 2));
		auto seGui = mpc->getUis().lock()->getStepEditorGui();
		auto res = eventNumber + seGui->getyOffset();
		seGui->setSelectionEndIndex(res);
	}
}

void GlobalReleaseControls::erase()
{
	auto controls = mpc->getControls().lock();
	controls->setErasePressed(false);
}

GlobalReleaseControls::~GlobalReleaseControls() {
}
