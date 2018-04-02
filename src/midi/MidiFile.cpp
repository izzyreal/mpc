#include <midi/MidiFile.hpp>
#include <midi/MidiTrack.hpp>
#include <midi/util/MidiUtil.hpp>

#include <io/BufferedInputStream.hpp>
#include <io/BufferedOutputStream.hpp>
#include <io/FileOutputStream.hpp>
#include <io/FileInputStream.hpp>
#include <io/CachedOutputStream.hpp>

using namespace mpc::midi;
using namespace std;

MidiFile::MidiFile() 
	: MidiFile(DEFAULT_RESOLUTION)
{
}

MidiFile::MidiFile(int resolution) 
: MidiFile(resolution, vector<MidiTrack*>(0))
{
}

MidiFile::MidiFile(int resolution, vector<MidiTrack*> tracks)
{
	mResolution = resolution >= 0 ? resolution : DEFAULT_RESOLUTION;
	mTrackCount = tracks.size();
	mType = mTrackCount > 1 ? 1 : 0;
}

MidiFile::MidiFile(unique_ptr<moduru::io::InputStream> rawIn)
{
	auto in = moduru::io::BufferedInputStream(rawIn.get());
	auto buffer = vector<char>(HEADER_SIZE);
	in.read(&buffer);
	initFromBuffer(buffer);
	mTracks.clear();
	for (int i = 0; i < mTrackCount; i++) {
		mTracks.push_back(std::move(make_shared<MidiTrack>(&in)));
	}
}

MidiFile::MidiFile(weak_ptr<moduru::file::File> fileIn)
	: MidiFile(make_unique<moduru::io::FileInputStream>(fileIn))
{
}

vector<char> MidiFile::IDENTIFIER = { 'M', 'T', 'h', 'd' };

void MidiFile::setType(int type)
{
	if (type < 0) {
		type = 0;
	}
	else if (type > 2) {
		type = 1;
	}
	else if (type == 0 && mTrackCount > 1) {
		type = 1;
	}
	mType = type;
}

int MidiFile::getType()
{
    return mType;
}

int MidiFile::getTrackCount()
{
    return mTrackCount;
}

void MidiFile::setResolution(int res)
{
	if (res >= 0) {
		mResolution = res;
	}
}

int MidiFile::getResolution()
{
    return mResolution;
}

int MidiFile::getLengthInTicks()
{
	int length = 0;
	for (auto& T : mTracks) {
		int l = T->getLengthInTicks();
		if (l > length) {
			length = l;
		}
	}
	return length;
}

vector<weak_ptr<MidiTrack>> MidiFile::getTracks()
{
	auto res = vector<weak_ptr<MidiTrack>>();
	for (auto& t : mTracks) {
		res.push_back(t);
	}
    return res;
}

void MidiFile::addTrack(shared_ptr<MidiTrack> T)
{
	addTrack(T, mTracks.size());
}

void MidiFile::addTrack(shared_ptr<MidiTrack> T, int pos)
{
	if (pos > mTracks.size()) {
		pos = mTracks.size();
	}
	else if (pos < 0) {
		pos = 0;
	}
	mTracks.insert(mTracks.begin() + pos, std::move(T));
	mTrackCount = mTracks.size();
	mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::removeTrack(int pos)
{
	if (pos < 0 || pos >= mTracks.size()) {
		return;
	}
	mTracks.erase(mTracks.begin() + pos);
	mTrackCount = mTracks.size();
	mType = mTrackCount > 1 ? 1 : 0;
}

void MidiFile::writeToFile(moduru::file::File* outFile)
{
    auto fout = new moduru::io::FileOutputStream(outFile);
    fout->write(IDENTIFIER);
    fout->write(midi::util::MidiUtil::intToBytes(6, 4));
    fout->write(midi::util::MidiUtil::intToBytes(mType, 2));
    fout->write(midi::util::MidiUtil::intToBytes(mTrackCount, 2));
    fout->write(midi::util::MidiUtil::intToBytes(mResolution, 2));
    for (auto& T : mTracks) {
		T->writeToFile(fout);
    }
    fout->flush();
    fout->close();
}

void MidiFile::initFromBuffer(vector<char> buffer)
{
    if(!mpc::midi::util::MidiUtil::bytesEqual(buffer, IDENTIFIER, 0, 4)) {
		string error = "File identifier not MThd. Exiting.\n";
        mType = 0;
        mTrackCount = 0;
        mResolution = DEFAULT_RESOLUTION;
        return;
    }
	mType = midi::util::MidiUtil::bytesToInt(buffer, 8, 2);
    mTrackCount = midi::util::MidiUtil::bytesToInt(buffer, 10, 2);
    mResolution = midi::util::MidiUtil::bytesToInt(buffer, 12, 2);
}

vector<char> MidiFile::getBytes()
{
	auto cos = moduru::io::CachedOutputStream();
	auto bos = moduru::io::BufferedOutputStream(&cos);
	bos.write(IDENTIFIER);
	bos.write(midi::util::MidiUtil::intToBytes(6, 4));
	bos.write(midi::util::MidiUtil::intToBytes(mType, 2));
	auto tc = midi::util::MidiUtil::intToBytes(mTrackCount, 2);
	bos.write(midi::util::MidiUtil::intToBytes(mTrackCount, 2));
	bos.write(midi::util::MidiUtil::intToBytes(mResolution, 2));
	for (auto& T : mTracks) {
		T->writeToFile(&bos);
	}
	bos.flush();
	bos.close();
	cos.flush();
	cos.close();
	return cos.get();
}

MidiFile::~MidiFile() {
}
