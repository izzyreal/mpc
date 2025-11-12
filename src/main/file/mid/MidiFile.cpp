#include "file/mid/MidiFile.hpp"
#include "file/mid/MidiTrack.hpp"
#include "file/mid/util/MidiUtil.hpp"

#include <Logger.hpp>

using namespace mpc::file::mid;
using namespace mpc::file::mid::util;

MidiFile::MidiFile(std::shared_ptr<std::istream> stream)
{
    std::vector<char> buffer(HEADER_SIZE);
    stream->read(&buffer[0], buffer.size());
    initFromBuffer(buffer);
    mTracks.clear();

    for (int i = 0; i < mTrackCount; i++)
    {
        mTracks.emplace_back(std::make_shared<MidiTrack>(stream));
    }
}

std::vector<char> MidiFile::IDENTIFIER = {'M', 'T', 'h', 'd'};

void MidiFile::setType(int type)
{
    if (type < 0)
    {
        type = 0;
    }
    else if (type > 2)
    {
        type = 1;
    }
    else if (type == 0 && mTrackCount > 1)
    {
        type = 1;
    }
    mType = type;
}

int MidiFile::getType() const
{
    return mType;
}

int MidiFile::getLengthInTicks() const
{
    int length = 0;
    for (auto &T : mTracks)
    {
        int l = T->getLengthInTicks();
        if (l > length)
        {
            length = l;
        }
    }
    return length;
}

std::vector<std::weak_ptr<MidiTrack>> MidiFile::getTracks() const
{
    auto res = std::vector<std::weak_ptr<MidiTrack>>();
    for (auto &t : mTracks)
    {
        res.push_back(t);
    }
    return res;
}

void MidiFile::addTrack(const std::shared_ptr<MidiTrack> &track)
{
    addTrack(track, mTracks.size());
}

void MidiFile::addTrack(std::shared_ptr<MidiTrack> track, int pos)
{
    if (pos > mTracks.size())
    {
        pos = mTracks.size();
    }
    else if (pos < 0)
    {
        pos = 0;
    }
    mTracks.insert(mTracks.begin() + pos, std::move(track));
    mTrackCount = mTracks.size();
    mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::writeToOutputStream(
    const std::shared_ptr<std::ostream> &stream) const
{
    stream->write(&IDENTIFIER[0], IDENTIFIER.size());

    auto val1 = MidiUtil::intToBytes(6, 4);
    stream->write(&val1[0], val1.size());

    auto type = MidiUtil::intToBytes(mType, 2);
    stream->write(&type[0], type.size());

    auto trackCount = MidiUtil::intToBytes(mTrackCount, 2);
    stream->write(&trackCount[0], trackCount.size());

    auto resolution = MidiUtil::intToBytes(mResolution, 2);
    stream->write(&resolution[0], resolution.size());

    for (auto &track : mTracks)
    {
        track->writeToOutputStream(stream);
    }
}

void MidiFile::initFromBuffer(std::vector<char> &buffer)
{
    if (!MidiUtil::bytesEqual(buffer, IDENTIFIER, 0, 4))
    {
        mType = 0;
        mTrackCount = 0;
        MLOG("File header does not indicate this is a MIDI file");
        return;
    }
    mType = MidiUtil::bytesToInt(buffer, 8, 2);
    mTrackCount = MidiUtil::bytesToInt(buffer, 10, 2);
    mResolution = MidiUtil::bytesToInt(buffer, 12, 2);

    if (mResolution != 96)
    {
        MLOG(
            "Developer warning: non-96ppq MIDI file was loaded. Most likely "
            "the event positions are incorrect.");
    }
}
