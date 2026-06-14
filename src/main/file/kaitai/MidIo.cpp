#include "file/kaitai/MidIo.hpp"

#include "Mpc.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/standard_midi_file_with_running_status.h"
#include "file/mid/MidiReader.hpp"
#include "file/mid/MidiWriter.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include <sstream>

using namespace mpc::file::kaitai;

sequence_or_error MidIo::load(mpc::Mpc &mpc, const std::shared_ptr<mpc::disk::MpcFile> &file)
{
    const auto bytes = file->getBytes();
    const auto canonicalBytes = parseRewrite<standard_midi_file_with_running_status_t>(bytes);

    auto newSeq = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    newSeq->init(0);

    auto input = std::make_shared<std::istringstream>(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::binary
    );
    mpc::file::mid::MidiReader reader(input, newSeq);
    reader.parseSequence(mpc);

    if (newSeq->getName().empty())
    {
        newSeq->setName(file->getNameWithoutExtension());
    }

    return newSeq;
}

void MidIo::save(const std::shared_ptr<mpc::sequencer::Sequence> &sequence,
                 const std::shared_ptr<std::ostream> &outputStream)
{
    mpc::file::mid::MidiWriter handwrittenWriter(sequence.get());
    auto handwrittenStream = std::make_shared<std::ostringstream>();
    handwrittenWriter.writeToOStream(handwrittenStream);
    const auto handwrittenBytesString = handwrittenStream->str();
    const std::vector<char> handwrittenBytes(
        handwrittenBytesString.begin(),
        handwrittenBytesString.end()
    );

    const auto canonicalBytes =
        parseRewrite<standard_midi_file_with_running_status_t>(handwrittenBytes);

    outputStream->write(canonicalBytes.data(), static_cast<std::streamsize>(canonicalBytes.size()));
}
