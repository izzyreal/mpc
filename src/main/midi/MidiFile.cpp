#include <midi/MidiFile.hpp>
#include <midi/MidiTrack.hpp>
#include <midi/util/MidiUtil.hpp>

#include <io/BufferedInputStream.hpp>
#include <io/FileInputStream.hpp>

#include <file/FileUtil.hpp>

#include <sstream>

using namespace mpc::midi;
using namespace mpc::midi::util;
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

void MidiFile::writeToOutputStream(ostream& stream)
{
	stream.write(&IDENTIFIER[0], IDENTIFIER.size());
	auto val1 = MidiUtil::intToBytes(6, 4);
	stream.write(&val1[0], val1.size());
	auto type = MidiUtil::intToBytes(mType, 2);
	stream.write(&type[0], type.size());
	auto trackCount = MidiUtil::intToBytes(mTrackCount, 2);
	stream.write(&trackCount[0], trackCount.size());
	auto resolution = MidiUtil::intToBytes(mResolution, 2);
	stream.write(&resolution[0], resolution.size());

	for (auto& track : mTracks) {
		track->writeToOutputStream(stream);
	}
}

void MidiFile::writeToFile(moduru::file::File* outFile)
{
	auto fout = moduru::file::FileUtil::ofstreamw(outFile->getPath(), ios::out | ios::binary);
	writeToOutputStream(fout);
	fout.close();
}

void MidiFile::initFromBuffer(vector<char>& buffer)
{
    if(!mpc::midi::util::MidiUtil::bytesEqual(buffer, IDENTIFIER, 0, 4)) {
		string error = "File identifier not MThd. Exiting.\n";
        mType = 0;
        mTrackCount = 0;
        mResolution = DEFAULT_RESOLUTION;
        return;
    }
	mType = MidiUtil::bytesToInt(buffer, 8, 2);
    mTrackCount = MidiUtil::bytesToInt(buffer, 10, 2);
    mResolution = MidiUtil::bytesToInt(buffer, 12, 2);
}

vector<char> MidiFile::getBytes()
{
	ostringstream stream;
	writeToOutputStream(stream);
	auto buffer = stream.str();
	return vector<char>(buffer.begin(), buffer.end());
}

MidiFile::~MidiFile() {
}
