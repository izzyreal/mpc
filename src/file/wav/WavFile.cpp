#include "WavFile.hpp"

using namespace mpc::file::wav;
using namespace std;

WavFile::WavFile() 
{
	buffer = vector<char>(BUFFER_SIZE);
}

const int WavFile::BUFFER_SIZE;
const int WavFile::FMT_CHUNK_ID;
const int WavFile::DATA_CHUNK_ID;
const int WavFile::RIFF_CHUNK_ID;
const int WavFile::RIFF_TYPE_ID;

int WavFile::getNumChannels()
{
    return numChannels;
}

int WavFile::getNumFrames()
{
    return numFrames;
}

int WavFile::getFramesRemaining()
{
    return numFrames - frameCounter;
}

int WavFile::getSampleRate()
{
    return sampleRate;
}

int WavFile::getValidBits()
{
    return validBits;
}

WavFile* WavFile::newWavFile(int numChannels, int numFrames, int validBits, int sampleRate) 
{   
    this->numChannels = numChannels;
    this->numFrames = numFrames;
    this->sampleRate = sampleRate;
    bytesPerSample = (validBits + 7) / 8;
    blockAlign = bytesPerSample * numChannels;
    this->validBits = validBits;
	if(numChannels < 1 || numChannels > 65535)
        throw std::invalid_argument("Illegal number of channels, valid range 1 to 65536");

    if(numFrames < 0)
        throw std::invalid_argument("Number of frames must be positive");

    if(validBits < 2 || validBits > 65535)
        throw std::invalid_argument("Illegal number of valid bits, valid range 2 to 65536");

    if(sampleRate < 0)
        throw std::invalid_argument("Sample rate must be positive");

    cache = make_unique<moduru::io::CachedOutputStream>();
    oStream = make_unique<moduru::io::BufferedOutputStream>(cache.get());
    auto dataChunkSize = blockAlign * numFrames;
    auto mainChunkSize = 4 + 8 + 16+ 8+ dataChunkSize;
    if(dataChunkSize % 2 == 1) {
        mainChunkSize += 1;
        wordAlignAdjust = true;
    } else {
        wordAlignAdjust = false;
    }
    putLE(RIFF_CHUNK_ID, 0, 4);
    putLE(mainChunkSize, 4, 4);
    putLE(RIFF_TYPE_ID, 8, 4);
    oStream->write(buffer, 0, 12);
    auto averageBytesPerSecond = sampleRate * blockAlign;
    putLE(FMT_CHUNK_ID, 0, 4);
    putLE(16, 4, 4);
    putLE(1, 8, 2);
    putLE(numChannels, 10, 2);
    putLE(sampleRate, 12, 4);
    putLE(averageBytesPerSecond, 16, 4);
    putLE(blockAlign, 20, 2);
    putLE(validBits, 22, 2);
    oStream->write(buffer, 0, 24);
    putLE(DATA_CHUNK_ID, 0, 4);
    putLE(dataChunkSize, 4, 4);
	oStream->write(buffer, 0, 8);
	if(validBits > 8) {
        floatOffset = 0;
        floatScale = 2147483647 >> (32 - validBits); // long max value
    } else {
        floatOffset = 1;
        floatScale = 0.5 * ((1 << validBits) - 1);
    }
    bufferPointer = 0;
    bytesRead = 0;
    frameCounter = 0;
    //ioState = WavFile_IOState::WRITING;
    return this;
}

void WavFile::openWavFile(weak_ptr<moduru::file::File> file) 
{   
	auto lFile = file.lock();
    iStream = make_unique<moduru::io::FileInputStream>(file);
	auto bytesRead = iStream->read(&buffer, 0, 12);
    if(bytesRead != 12)
        throw std::invalid_argument("Not enough wav file bytes for header");
	
    auto riffChunkID = getLE(0, 4);
	auto chunkSize = getLE(4, 4);
	auto riffTypeID = getLE(8, 4);
	if(riffChunkID != RIFF_CHUNK_ID)
        throw std::invalid_argument("Invalid Wav Header data, incorrect riff chunk ID");

    if(riffTypeID != RIFF_TYPE_ID)
        throw std::invalid_argument("Invalid Wav Header data, incorrect riff type ID");

    if(lFile->getLength() != chunkSize + 8) {
        chunkSize = lFile->getLength();
    }
    auto foundFormat = false;
    auto foundData = false;
    while (true) {
        bytesRead = iStream->read(&buffer, 0, 8);
        if(bytesRead == -1)
            throw std::invalid_argument("Reached end of file without finding format chunk");

        if(bytesRead != 8)
            throw std::invalid_argument("Could not read chunk header");

        auto chunkID = getLE(0, 4);
        chunkSize = getLE(4, 4);
        auto numChunkBytes = (chunkSize % 2 == 1) ? chunkSize + 1 : chunkSize;
		if(chunkID == FMT_CHUNK_ID) {
            foundFormat = true;
            bytesRead = iStream->read(&buffer, 0, 16);
            auto compressionCode = static_cast< int >(getLE(0, 2));
			if (compressionCode != 1) {
				string exc = "Compression Code " + to_string(compressionCode)+ " not supported";
				throw std::invalid_argument(exc.c_str());
			}

            numChannels = static_cast< int >(getLE(2, 2));
            sampleRate = getLE(4, 4);
            blockAlign = static_cast< int >(getLE(12, 2));
            validBits = static_cast< int >(getLE(14, 2));
			if(numChannels == 0)
                throw std::invalid_argument("Number of channels specified in header is equal to zero");

            if(blockAlign == 0)
                throw std::invalid_argument("Block Align specified in header is equal to zero");

            if(validBits < 2)
                throw std::invalid_argument("Valid Bits specified in header is less than 2");

            if(validBits > 64)
                throw std::invalid_argument("Valid Bits specified in header is greater than 64, this is greater than a long can hold");

            bytesPerSample = (validBits + 7) / 8;
            if(bytesPerSample * numChannels != blockAlign)
                throw std::invalid_argument("Block Align does not agree with bytes required for validBits and number of channels");

            numChunkBytes -= 16;
            if(numChunkBytes > 0)
                iStream->skip(numChunkBytes);

        } else if(chunkID == DATA_CHUNK_ID) {
            if(foundFormat == false)
                throw std::invalid_argument("Data chunk found before Format chunk");

            if(chunkSize % blockAlign != 0)
                throw std::invalid_argument("Data Chunk size is not multiple of Block Align");

            numFrames = chunkSize / blockAlign;
            foundData = true;
            break;
        } else {
            iStream->skip(numChunkBytes);
        }
    }
    if(foundData == false)
        throw std::invalid_argument("Did not find a data chunk");

    if(validBits > 8) {
        floatOffset = 0;
        floatScale = 1 << (validBits - 1);
    } else {
        floatOffset = -1;
        floatScale = 0.5 * ((1 << validBits) - 1);
    }

    bufferPointer = 0;
    bytesRead = 0;
    frameCounter = 0;
    //ioState = WavFile_IOState::READING;
}

int WavFile::getLE(int pos, int numBytes)
{
	numBytes--;
	pos += numBytes;
	int val = buffer[pos] & 255;
	for (auto b = 0; b < numBytes; b++)
		val = (val << 8) + (buffer[--pos] & 255);
	return val;
}

void WavFile::putLE(int val, int pos, int numBytes)
{
	for (auto b = 0; b < numBytes; b++) {
		buffer[pos] = static_cast<char>(val & 255);
		val >>= 8;
		pos++;
	}
}

void WavFile::writeSample(int val)
{
	for (auto b = 0; b < bytesPerSample; b++) {
		if (bufferPointer == BUFFER_SIZE) {
			oStream->write(buffer, 0, BUFFER_SIZE);
			bufferPointer = 0;
		}
		buffer[bufferPointer] = static_cast<int8_t>((val & 255));
		val >>= 8;
		bufferPointer++;
	}
}

int WavFile::readSample()
{
	int val = 0;
	for (auto b = 0; b < bytesPerSample; b++) {
		if (bufferPointer == bytesRead) {
			auto read = iStream->read(&buffer, 0, BUFFER_SIZE);
			if (read == -1)
				throw std::invalid_argument("Not enough data available");

			bytesRead = read;
			bufferPointer = 0;
		}
		int v = buffer[bufferPointer];
		if (b < bytesPerSample - 1 || bytesPerSample == 1)
			v &= 255;
		val += v << (b * 8);
		bufferPointer++;
	}
	return val;
}

int WavFile::readFrames(vector<int>* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(vector<int>* sampleBuffer, int offset, int numFramesToRead)
{
	//   if(ioState != WavFile_IOState::READING)
	  //     throw std::invalid_argument("Cannot read from WavFile instance");

	for (auto f = 0; f < numFramesToRead; f++) {
		if (frameCounter == numFrames)
			return f;

		for (auto c = 0; c < numChannels; c++) {
			(*sampleBuffer)[offset] = static_cast<int>(readSample());
			offset++;
		}
		frameCounter++;
	}
	return numFramesToRead;
}

int WavFile::readFrames(vector<vector<int>>* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(vector<vector<int>>* sampleBuffer, int offset, int numFramesToRead)
{
	//   if(ioState != WavFile_IOState::READING)
	 //      throw new moduru::io::IOException("Cannot read from WavFile instance");

	for (auto f = 0; f < numFramesToRead; f++) {
		if (frameCounter == numFrames)
			return f;

		for (auto c = 0; c < numChannels; c++)
			(*sampleBuffer)[c][offset] = static_cast<int>(readSample());

		offset++;
		frameCounter++;
	}
	return numFramesToRead;
}

int WavFile::writeFrames(vector<int>* sampleBuffer, int numFramesToWrite) 
{
    return writeFrames(sampleBuffer, 0, numFramesToWrite);
}

int WavFile::writeFrames(vector<int>* sampleBuffer, int offset, int numFramesToWrite)
{
	//  if(ioState != WavFile_IOState::WRITING)
	 //     throw new moduru::io::IOException("Cannot write to WavFile instance");

	for (auto f = 0; f < numFramesToWrite; f++) {
		if (frameCounter == numFrames)
			return f;

		for (auto c = 0; c < numChannels; c++) {
			writeSample((*sampleBuffer)[offset]);
			offset++;
		}
		frameCounter++;
	}
	return numFramesToWrite;
}

int WavFile::writeFrames(vector<vector<int>>* sampleBuffer, int numFramesToWrite) 
{
    return writeFrames(sampleBuffer, 0, numFramesToWrite);
}

int WavFile::writeFrames(vector<vector<int>>* sampleBuffer, int offset, int numFramesToWrite)
{
	//if(ioState != WavFile_IOState::WRITING)
//        throw new moduru::io::IOException("Cannot write to WavFile instance");

	for (int f = 0; f < numFramesToWrite; f++) {
		if (frameCounter == numFrames)
			return f;

		for (int c = 0; c < numChannels; c++)
			writeSample((*sampleBuffer)[c][offset]);

		offset++;
		frameCounter++;
	}
	return numFramesToWrite;
}

/*
int WavFile::readFrames(::int64_tArray* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(::int64_tArray* sampleBuffer, int offset, int numFramesToRead) 
{
    if(ioState != WavFile_IOState::READING)
        throw new moduru::io::IOException("Cannot read from WavFile instance");

    for (auto f = 0; f < numFramesToRead; f++) {
        if(frameCounter == numFrames)
            return f;

        for (auto c = 0; c < numChannels; c++) {
            (*sampleBuffer)[offset] = readSample();
            offset++;
        }
        frameCounter++;
    }
    return numFramesToRead;
}

int WavFile::readFrames(::int64_tArrayArray* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(::int64_tArrayArray* sampleBuffer, int offset, int numFramesToRead) 
{
    if(ioState != WavFile_IOState::READING)
        throw new moduru::io::IOException("Cannot read from WavFile instance");

    for (auto f = 0; f < numFramesToRead; f++) {
        if(frameCounter == numFrames)
            return f;

        for (auto c = 0; c < numChannels; c++) 
                        (*(*sampleBuffer)[c])[offset] = readSample();

        offset++;
        frameCounter++;
    }
    return numFramesToRead;
}

int WavFile::writeFrames(::int64_tArray* sampleBuffer, int numFramesToWrite) 
{
    return writeFrames(sampleBuffer, 0, numFramesToWrite);
}

int WavFile::writeFrames(::int64_tArray* sampleBuffer, int offset, int numFramesToWrite) 
{
    if(ioState != WavFile_IOState::WRITING)
        throw new moduru::io::IOException("Cannot write to WavFile instance");

    for (auto f = 0; f < numFramesToWrite; f++) {
        if(frameCounter == numFrames)
            return f;

        for (auto c = 0; c < numChannels; c++) {
            writeSample((*sampleBuffer)[offset]);
            offset++;
        }
        frameCounter++;
    }
    return numFramesToWrite;
}

int WavFile::writeFrames(::int64_tArrayArray* sampleBuffer, int numFramesToWrite) 
{
    return writeFrames(sampleBuffer, 0, numFramesToWrite);
}

int WavFile::writeFrames(::int64_tArrayArray* sampleBuffer, int offset, int numFramesToWrite) 
{
    if(ioState != WavFile_IOState::WRITING)
        throw new moduru::io::IOException("Cannot write to WavFile instance");

    for (auto f = 0; f < numFramesToWrite; f++) {
        if(frameCounter == numFrames)
            return f;

        for (auto c = 0; c < numChannels; c++) 
                        writeSample((*(*sampleBuffer)[c])[offset]);

        offset++;
        frameCounter++;
    }
    return numFramesToWrite;
}
*/

int WavFile::readFrames(vector<float>* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(vector<float>* sampleBuffer, int offset, int numFramesToRead)
{
	//if(ioState != WavFile_IOState::READING)
		//throw new moduru::io::IOException("Cannot read from WavFile instance");
	if (sampleBuffer->size() != numFramesToRead * numChannels) {
		sampleBuffer->resize(numFramesToRead * numChannels);
	}

	for (auto f = 0; f < numFramesToRead; f++) {
		if (frameCounter == numFrames)
			return f;
		for (auto c = 0; c < numChannels; c++) {
			auto v = readSample();
			(*sampleBuffer)[offset] = floatOffset + static_cast<double>(v) / floatScale;
			offset++;
		}
		frameCounter++;
	}
	return numFramesToRead;
}

int WavFile::readFrames(vector<vector<float>>* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(vector<vector<float>>* sampleBuffer, int offset, int numFramesToRead)
{
	//if(ioState != WavFile_IOState::READING)
		//throw new moduru::io::IOException("Cannot read from WavFile instance");

	for (auto f = 0; f < numFramesToRead; f++) {
		if (frameCounter == numFrames)
			return f;

		for (auto c = 0; c < numChannels; c++)
			(*sampleBuffer)[c][offset] = floatOffset + static_cast<double>(readSample()) / floatScale;

		offset++;
		frameCounter++;
	}
	return numFramesToRead;
}

int WavFile::writeFrames(vector<float>* sampleBuffer, int numFramesToWrite) 
{
    return writeFrames(sampleBuffer, 0, numFramesToWrite);
}

int WavFile::writeFrames(vector<float>* sampleBuffer, int offset, int numFramesToWrite) 
{
 //   if(ioState != WavFile_IOState::WRITING)
  //      throw new moduru::io::IOException("Cannot write to WavFile instance");

    for (auto f = 0; f < numFramesToWrite; f++) {
        if(frameCounter == numFrames)
            return f;

        for (auto c = 0; c < numChannels; c++) {
            writeSample(static_cast< int >((floatScale * (floatOffset + (*sampleBuffer)[offset]))));
            offset++;
        }
        frameCounter++;
    }
    return numFramesToWrite;
}

int WavFile::writeFrames(vector<vector<float>>* sampleBuffer, int numFramesToWrite) 
{
    return writeFrames(sampleBuffer, 0, numFramesToWrite);
}

int WavFile::writeFrames(vector<vector<float>>* sampleBuffer, int offset, int numFramesToWrite)
{
	//if(ioState != WavFile_IOState::WRITING)
	 //   throw new moduru::io::IOException("Cannot write to WavFile instance");

	for (auto f = 0; f < numFramesToWrite; f++) {
		if (frameCounter == numFrames)
			return f;

		for (auto c = 0; c < numChannels; c++)
			writeSample(static_cast<int>((floatScale * (floatOffset + (*sampleBuffer)[c][offset]))));

		offset++;
		frameCounter++;
	}
	return numFramesToWrite;
}

void WavFile::close() 
{
    if(iStream != nullptr) {
        iStream->close();
    }
    if(oStream != nullptr) {
        if(bufferPointer > 0)
            oStream->write(buffer, 0, bufferPointer);

        if(wordAlignAdjust)
            oStream->write(0);

        oStream->close();
    }
    //ioState = WavFile_IOState::CLOSED;
}

vector<char> WavFile::getResult()
{
    return cache->get();
}
