#include "AllSequencer.hpp"

#include <Mpc.hpp>

#include <sequencer/Sequencer.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SecondSeqScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::file::all;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

AllSequencer::AllSequencer(mpc::Mpc& mpc, const vector<char>& loadBytes)
	: mpc(mpc)
{
	sequence = loadBytes[SEQ_OFFSET];
	track = loadBytes[TR_OFFSET];
	tc = loadBytes[TC_OFFSET];
	secondSeqEnabled = loadBytes[SECOND_SEQ_ENABLED_OFFSET] > 0;
	secondSeqIndex = loadBytes[SECOND_SEQ_INDEX_OFFSET];
}

AllSequencer::AllSequencer(mpc::Mpc& mpc)
	: mpc(mpc)
{
	saveBytes = vector<char>(LENGTH);
	for (int i = 0; i < LENGTH; i++)
		saveBytes[i] = TEMPLATE[i];
	auto seq = mpc.getSequencer().lock();
	saveBytes[SEQ_OFFSET] = seq->getActiveSequenceIndex();
	saveBytes[TR_OFFSET] = seq->getActiveTrackIndex();

	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
	auto noteValue = timingCorrectScreen->getNoteValue();

	saveBytes[TC_OFFSET] = noteValue;
	saveBytes[SECOND_SEQ_ENABLED_OFFSET] = seq->isSecondSequenceEnabled() ? 1 : 0;
	
	auto secondSequenceScreen = mpc.screens->get<SecondSeqScreen>("second-seq");
	saveBytes[SECOND_SEQ_INDEX_OFFSET] = secondSequenceScreen->sq;
}

vector<char> AllSequencer::TEMPLATE = vector<char>{ 0, 0, 0, 0 , (char) 176, 4, 1, 3, 0, 0, 0, 0, 12, 0, 0, 0 };

vector<char>& AllSequencer::getBytes()
{
    return saveBytes;
}
