#include "LoadASequenceScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc::lcdgui::screens::window;

LoadASequenceScreen::LoadASequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-a-sequence", layerIndex)
{
}

void LoadASequenceScreen::open()
{
    if (mpc.getLayeredScreen()->isPreviousScreen(
            {ScreenId::LoadASequencePlayScreen}))
    {
        displayLoadInto();
        return;
    }

    originalSelectedSequenceIndex =
        sequencer.lock()->getSelectedSequenceIndex();
    previewingTempSequence = false;

    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto sequenceFile = loadScreen->getSelectedFile();

    if (!sequenceFile)
    {
        return;
    }

    const auto extension = sequenceFile->getExtension();

    if (!StrUtil::eqIgnoreCase(extension, ".mid") &&
        !StrUtil::eqIgnoreCase(extension, ".seq"))
    {
        return;
    }

    const sequence_or_error parsedSequenceFile =
        StrUtil::eqIgnoreCase(extension, ".seq")
            ? mpc.getDisk()->readSeq2(sequenceFile)
            : mpc.getDisk()->readMid2(sequenceFile);

    if (parsedSequenceFile.has_value())
    {
        auto usedSeqs = sequencer.lock()->getUsedSequenceIndexes();
        int index;

        for (index = MinSequenceIndex; index <= MaxSequenceIndex; index += 1)
        {
            if (find(begin(usedSeqs), end(usedSeqs), index) == end(usedSeqs))
            {
                break;
            }
        }

        loadInto = SequenceIndex(index);
        displayFile();
    }

    displayLoadInto();
}

void LoadASequenceScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "load-into")
    {
        setLoadInto(loadInto + increment);
    }
}

void LoadASequenceScreen::function(const int i)
{
    switch (i)
    {
        case 2:
            startTempPreview();
            break;
        case 3:
            stopTempPreviewAndRestoreSelection();
            mpc.getLayeredScreen()->closeCurrentScreen();
            sequencer.lock()->getSequence(TempSequenceIndex)->init(0);
            break;
        case 4:
        {
            const auto lockedSequencer = sequencer.lock();
            if (lockedSequencer->getTransport()->isPlaying())
            {
                lockedSequencer->getTransport()->stop();
            }
            lockedSequencer->copySequence(TempSequenceIndex, loadInto);
            lockedSequencer->setSelectedSequenceIndex(loadInto, true);
            previewingTempSequence = false;
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        default:;
    }
}

void LoadASequenceScreen::startTempPreview()
{
    auto lockedSequencer = sequencer.lock();
    auto stateManager = lockedSequencer->getStateManager();
    auto transport = lockedSequencer->getTransport();

    if (!previewingTempSequence)
    {
        originalSelectedSequenceIndex =
            lockedSequencer->getSelectedSequenceIndex();
    }

    if (transport->isPlaying())
    {
        transport->stop();
    }

    stateManager->enqueue(
        mpc::sequencer::SetSelectedSequenceIndex{TempSequenceIndex, true});
    transport->play(true);
    previewingTempSequence = true;
    openScreenById(ScreenId::LoadASequencePlayScreen);
}

void LoadASequenceScreen::stopTempPreviewAndRestoreSelection()
{
    auto lockedSequencer = sequencer.lock();
    auto transport = lockedSequencer->getTransport();

    if (transport->isPlaying())
    {
        transport->stop();
    }

    if (!previewingTempSequence)
    {
        return;
    }

    lockedSequencer->getStateManager()->enqueue(
        mpc::sequencer::SetSelectedSequenceIndex{originalSelectedSequenceIndex,
                                                 true});
    previewingTempSequence = false;
}

void LoadASequenceScreen::releasePlayPreview()
{
    stopTempPreviewAndRestoreSelection();
}

void LoadASequenceScreen::setLoadInto(const SequenceIndex i)
{
    loadInto = std::clamp(i, MinSequenceIndex, MaxSequenceIndex);
    displayLoadInto();
}

void LoadASequenceScreen::displayLoadInto() const
{
    findField("load-into")->setTextPadded(loadInto + 1, "0");
    findLabel("name")->setText(
        "-" + sequencer.lock()->getSequence(loadInto)->getName());
}

void LoadASequenceScreen::displayFile() const
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto sequenceFile = loadScreen->getSelectedFile();
    findLabel("file")->setText(
        "File:" +
        StrUtil::padRight(
            StrUtil::toUpper(sequenceFile->getNameWithoutExtension()), " ",
            16) +
        StrUtil::toUpper(sequenceFile->getExtension()));
}
