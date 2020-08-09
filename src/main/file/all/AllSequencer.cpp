#include <file/all/AllSequencer.hpp>

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

mpc::file::all::Sequencer::Sequencer(mpc::Mpc& mpc, vector<char> loadBytes)
	: mpc(mpc)
{
	sequence = loadBytes[SEQ_OFFSET];
	track = loadBytes[TR_OFFSET];
	tc = loadBytes[TC_OFFSET];
	secondSeqEnabled = loadBytes[SECOND_SEQ_ENABLED_OFFSET] > 0;
	secondSeqIndex = loadBytes[SECOND_SEQ_INDEX_OFFSET];
}

mpc::file::all::Sequencer::Sequencer(mpc::Mpc& mpc)
	: mpc(mpc)
{
	saveBytes = vector<char>(LENGTH);
	for (int i = 0; i < LENGTH; i++)
		saveBytes[i] = TEMPLATE[i];
	auto seq = mpc.getSequencer().lock();
	saveBytes[SEQ_OFFSET] = seq->getActiveSequenceIndex();
	saveBytes[TR_OFFSET] = seq->getActiveTrackIndex();

	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(mpc.screens->getScreenComponent("timing-correct"));
	auto noteValue = timingCorrectScreen->getNoteValue();

	saveBytes[TC_OFFSET] = noteValue;
	saveBytes[SECOND_SEQ_ENABLED_OFFSET] = seq->isSecondSequenceEnabled() ? 1 : 0;
	
	auto secondSequenceScreen = dynamic_pointer_cast<SecondSeqScreen>(mpc.screens->getScreenComponent("second-seq"));
	saveBytes[SECOND_SEQ_INDEX_OFFSET] = secondSequenceScreen->sq;
}

const int mpc::file::all::Sequencer::LENGTH;
vector<char> mpc::file::all::Sequencer::TEMPLATE = vector<char>{ 0, 0, 0, 0 , (char) 176, 4, 1, 3, 0, 0, 0, 0, 12, 0, 0, 0 };
const int mpc::file::all::Sequencer::SEQ_OFFSET;
const int mpc::file::all::Sequencer::TR_OFFSET;
const int mpc::file::all::Sequencer::TC_OFFSET;
const int mpc::file::all::Sequencer::SECOND_SEQ_ENABLED_OFFSET;
const int mpc::file::all::Sequencer::SECOND_SEQ_INDEX_OFFSET;

vector<char> mpc::file::all::Sequencer::getBytes()
{
    return saveBytes;
}
