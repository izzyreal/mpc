#include "AllSequencer.hpp"
#include "sequencer/Transport.hpp"
#include "file/ByteUtil.hpp"
#include "lcdgui/screens/window/TimeDisplayScreen.hpp"

#include "Mpc.hpp"

#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::file::all;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

AllSequencer::AllSequencer(const std::vector<char> &loadBytes)
{
    sequence = loadBytes[SEQ_OFFSET];
    track = loadBytes[TR_OFFSET];
    const auto masterTempoBytes = {loadBytes[MASTER_TEMPO_OFFSET],
                                   loadBytes[MASTER_TEMPO_OFFSET + 1]};
    masterTempo = ByteUtil::bytes2ushort(masterTempoBytes) / 10.0;
    tempoSourceIsSequence = loadBytes[TEMPO_SOURCE_IS_SEQUENCE_OFFSET] == 0x01;
    tc = loadBytes[TC_OFFSET];
    timeDisplayStyle = loadBytes[TIME_DISPLAY_STYLE_OFFSET];
    secondSeqEnabled = loadBytes[SECOND_SEQ_ENABLED_OFFSET] > 0;
    secondSeqIndex = loadBytes[SECOND_SEQ_INDEX_OFFSET];
}

AllSequencer::AllSequencer(mpc::Mpc &mpc)
{
    saveBytes = std::vector<char>(LENGTH);

    for (int i = 0; i < LENGTH; i++)
    {
        saveBytes[i] = TEMPLATE[i];
    }

    const auto mpcSequencer = mpc.getSequencer();

    saveBytes[SEQ_OFFSET] = mpcSequencer->getActiveSequenceIndex();
    saveBytes[TR_OFFSET] = mpcSequencer->getActiveTrackIndex();

    const bool tempoSourceIsSequence =
        mpcSequencer->getTransport()->isTempoSourceSequenceEnabled();

    mpcSequencer->getTransport()->setTempoSourceSequence(false);

    const auto masterTempoBytes =
        ByteUtil::ushort2bytes(mpcSequencer->getTransport()->getTempo() * 10.0);

    mpcSequencer->getTransport()->setTempoSourceSequence(tempoSourceIsSequence);

    saveBytes[MASTER_TEMPO_OFFSET] = masterTempoBytes[0];
    saveBytes[MASTER_TEMPO_OFFSET + 1] = masterTempoBytes[1];

    saveBytes[TEMPO_SOURCE_IS_SEQUENCE_OFFSET] =
        mpcSequencer->getTransport()->isTempoSourceSequenceEnabled() ? 0x01
                                                                     : 0x00;

    const auto timingCorrectScreen =
        mpc.screens->get<ScreenId::TimingCorrectScreen>();
    const auto noteValue = timingCorrectScreen->getNoteValue();

    saveBytes[TC_OFFSET] = noteValue;
    saveBytes[TIME_DISPLAY_STYLE_OFFSET] =
        mpc.screens->get<ScreenId::TimeDisplayScreen>()->getDisplayStyle();
    saveBytes[SECOND_SEQ_ENABLED_OFFSET] =
        mpcSequencer->isSecondSequenceEnabled() ? 1 : 0;

    auto secondSequenceScreen = mpc.screens->get<ScreenId::SecondSeqScreen>();
    saveBytes[SECOND_SEQ_INDEX_OFFSET] = secondSequenceScreen->sq;
}

std::vector<char> AllSequencer::TEMPLATE = {0, 0, 0, 0, (char)176, 4,
                                            1, 3, 0, 0, 0};

std::vector<char> &AllSequencer::getBytes()
{
    return saveBytes;
}
