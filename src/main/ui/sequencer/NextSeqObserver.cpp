#include <ui/sequencer/NextSeqObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/Field.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <thirdp/bcmath/bcmath_stl.h>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <lang/StrUtil.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::ui::sequencer;
using namespace std;

NextSeqObserver::NextSeqObserver() 
{
	
	sequencer = Mpc::instance().getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->deleteObservers();
	lSequencer->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	sqField = ls->lookupField("sq");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	now0Field.lock()->setFocusable(false);
	now1Field.lock()->setFocusable(false);
	now2Field.lock()->setFocusable(false);
	tempoField = ls->lookupField("tempo");
	tempoLabel = ls->lookupLabel("tempo");
	tempoSourceField = ls->lookupField("temposource");
	timingField = ls->lookupField("timing");
	nextSqField = ls->lookupField("nextsq");
}

void NextSeqObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("seqnumbername") == 0) {
		displaySq();
	}
	else if (s.compare("bar") == 0) {
		displayNow0();
	}
	else if (s.compare("beat") == 0) {
		displayNow1();
	}
	else if (s.compare("clock") == 0) {
		displayNow2();
	}
	else if (s.compare("now") == 0) {
		displayNow0();
		displayNow1();
		displayNow2();
	}
	else if (s.compare("nextsqvalue") == 0) {
		displayNextSq();
	}
	else if (s.compare("nextsq") == 0) {
		nextSqField.lock()->Hide(false);
		//mainFrame->setFocus(nextSqField.lock()->getName(), 0);
		displayNextSq();
	}
	else if (s.compare("nextsqoff") == 0) {
		nextSqField.lock()->Hide(true);
		//mainFrame->setFocus("sq", 0);
	}
	else if (s.compare("timing") == 0) {
		displayTiming();
	}
}

NextSeqObserver::~NextSeqObserver() {
	sequencer.lock()->deleteObserver(this);
}
