#include "file/kaitai/AllIo.hpp"

#include "Mpc.hpp"
#include "disk/AllLoader.hpp"
#include "disk/MpcFile.hpp"
#include "file/all/AllParser.hpp"
#include "file/all/AllSequence.hpp"
#include "file/all/SequenceNames.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "sequencer/Sequencer.hpp"

using namespace mpc::file::kaitai;

void AllIo::loadEverything(mpc::Mpc &mpc, mpc::disk::MpcFile *file)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_all_t>(file->getBytes());
    mpc::file::all::AllParser parser(mpc, canonicalBytes);
    mpc::disk::AllLoader::loadEverythingFromAllParser(mpc, parser);
}

sequence_meta_infos_or_error AllIo::loadSequenceMetaInfos(mpc::Mpc &mpc, mpc::disk::MpcFile *file)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_all_t>(file->getBytes());
    const mpc::file::all::AllParser parser(mpc, canonicalBytes);

    std::vector<mpc::sequencer::SequenceMetaInfo> result;
    const auto allSeqNames = parser.getSeqNames()->getNames();
    const auto allSeqUsednesses = parser.getSeqNames()->getUsednesses();

    for (int i = 0; i < mpc::Mpc2000XlSpecs::SEQUENCE_COUNT; ++i)
    {
        result.push_back({allSeqUsednesses[i], allSeqNames[i]});
    }

    return result;
}

std::shared_ptr<mpc::sequencer::Sequence> AllIo::loadOneSequence(
    mpc::Mpc &mpc,
    mpc::disk::MpcFile *file,
    mpc::SequenceIndex sourceIndexInAllFile,
    mpc::SequenceIndex destIndexInMpcMemory)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_all_t>(file->getBytes());
    mpc::file::all::AllParser parser(mpc, canonicalBytes);

    const auto sequence = mpc.getSequencer()->getSequence(destIndexInMpcMemory);
    parser.getAllSequences()[sourceIndexInAllFile]->applyToInMemorySequence(
        sequence,
        mpc.getSequencer()->getStateManager().get()
    );

    return sequence;
}

std::vector<char> AllIo::save(mpc::Mpc &mpc)
{
    mpc::file::all::AllParser parser(mpc);
    return parseRewrite<mpc2000xl_all_t>(parser.getBytes());
}
