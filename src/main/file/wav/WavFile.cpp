#include "WavFile.hpp"

#include <file/FileUtil.hpp>

using namespace mpc::file::wav;
using namespace std;

using namespace moduru::file;

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

WavFile WavFile::newWavFile(const string& path, int numChannels, int numFrames, int validBits, int sampleRate) 
{   
    WavFile result;
    result.oStream = FileUtil::ofstreamw(path, ios::out | ios::binary);
    result.numChannels = numChannels;
    result.numFrames = numFrames;
    result.sampleRate = sampleRate;
    result.bytesPerSample = (validBits + 7) / 8;
    result.blockAlign = result.bytesPerSample * numChannels;
    result.validBits = validBits;

    if (numChannels < 1 || numChannels > 65535) {
        throw std::invalid_argument("Illegal number of channels, valid range 1 to 65536");
    }

    if (numFrames < 0) {
        throw std::invalid_argument("Number of frames must be positive");
    }

    if (validBits < 2 || validBits > 65535) {
        throw std::invalid_argument("Illegal number of valid bits, valid range 2 to 65536");
    }

    if (sampleRate < 0) {
        throw std::invalid_argument("Sample rate must be positive");
    }

    auto dataChunkSize = result.blockAlign * numFrames;
    auto mainChunkSize = 4 + 8 + 16+ 8+ dataChunkSize;
    
    if (dataChunkSize % 2 == 1) {
        mainChunkSize += 1;
        result.wordAlignAdjust = true;
    }
    else {
        result.wordAlignAdjust = false;
    }
    
    putLE(RIFF_CHUNK_ID, result.buffer, 0, 4);
    putLE(mainChunkSize, result.buffer, 4, 4);
    putLE(RIFF_TYPE_ID, result.buffer, 8, 4);

    result.oStream.write(&result.buffer[0], 12);

    auto averageBytesPerSecond = sampleRate * result.blockAlign;
    putLE(FMT_CHUNK_ID, result.buffer, 0, 4);
    putLE(16, result.buffer, 4, 4);
    putLE(1, result.buffer, 8, 2);
    putLE(numChannels, result.buffer, 10, 2);
    putLE(sampleRate, result.buffer, 12, 4);
    putLE(averageBytesPerSecond, result.buffer, 16, 4);
    putLE(result.blockAlign, result.buffer, 20, 2);
    putLE(validBits, result.buffer, 22, 2);
    
    result.oStream.write(&result.buffer[0], 24);

    putLE(DATA_CHUNK_ID, result.buffer, 0, 4);
    putLE(dataChunkSize, result.buffer, 4, 4);
	
    result.oStream.write(&result.buffer[0], 8);

	if(validBits > 8) {
        result.floatOffset = 0;
        result.floatScale = 2147483647 >> (32 - validBits); // long max value
    } else {
        result.floatOffset = 1;
        result.floatScale = 0.5 * ((1 << validBits) - 1);
    }
    result.bufferPointer = 0;
    result.bytesRead = 0;
    result.frameCounter = 0;
    return result;
}

WavFile WavFile::openWavFile(const string& path) 
{   
    WavFile result;
    result.iStream = FileUtil::ifstreamw(path, ios::in | ios::binary);
    
    result.iStream.seekg(0, ios::end);

    auto fileSize = result.iStream.tellg();

    result.iStream.seekg(0, ios::beg);
    
    result.iStream.read(&result.buffer[0], 12);
    
    auto bytesRead = result.iStream.gcount();

    if (bytesRead != 12) {
        result.iStream.close();
        throw std::invalid_argument("Not enough wav file bytes for header");
    }
	
    auto riffChunkID = getLE(result.buffer, 0, 4);
	auto chunkSize = getLE(result.buffer, 4, 4);
	auto riffTypeID = getLE(result.buffer, 8, 4);

    if (riffChunkID != RIFF_CHUNK_ID) {
        result.iStream.close();
        throw std::invalid_argument("Invalid Wav Header data, incorrect riff chunk ID");
    }

    if (riffTypeID != RIFF_TYPE_ID) {
        result.iStream.close();
        throw std::invalid_argument("Invalid Wav Header data, incorrect riff type ID");
    }

    if (chunkSize % 2 != 0) {
        result.iStream.close();
        chunkSize += 1;
    }

    if (fileSize != chunkSize + 8) {
        result.iStream.close();
        throw std::invalid_argument("Invalid Wav Header data, incorrect chunk size");
    }

    auto foundFormat = false;
    auto foundData = false;

    while (true) {
        result.iStream.read(&result.buffer[0], 8);
        bytesRead = result.iStream.gcount();
        
        if (bytesRead != 8) {
            result.iStream.close();
            throw std::invalid_argument("Could not read chunk header");
        }

        auto chunkID = getLE(result.buffer, 0, 4);
        chunkSize = getLE(result.buffer, 4, 4);
        auto numChunkBytes = (chunkSize % 2 == 1) ? chunkSize + 1 : chunkSize;

		if (chunkID == FMT_CHUNK_ID) {
            foundFormat = true;
            result.iStream.read(&result.buffer[0], 16);
            bytesRead = result.iStream.gcount();
            auto compressionCode = static_cast< int >(getLE(result.buffer, 0, 2));
			if (compressionCode != 1) {
				string exc = "Compression Code " + to_string(compressionCode)+ " not supported";
                result.iStream.close();
				throw std::invalid_argument(exc.c_str());
			}

            result.numChannels = static_cast< int >(getLE(result.buffer, 2, 2));
            result.sampleRate = getLE(result.buffer, 4, 4);
            result.blockAlign = static_cast< int >(getLE(result.buffer, 12, 2));
            result.validBits = static_cast< int >(getLE(result.buffer, 14, 2));
			
            if (result.numChannels == 0) {
                result.iStream.close();
                throw std::invalid_argument("Number of channels specified in header is equal to zero");
            }

            if (result.blockAlign == 0) {
                result.iStream.close();
                throw std::invalid_argument("Block Align specified in header is equal to zero");
            }

            if (result.validBits < 2) {
                result.iStream.close();
                throw std::invalid_argument("Valid Bits specified in header is less than 2");
            }

            if (result.validBits > 64) {
                result.iStream.close();
                throw std::invalid_argument("Valid Bits specified in header is greater than 64, this is greater than a long can hold");
            }

            result.bytesPerSample = (result.validBits + 7) / 8;

            if (result.bytesPerSample * result.numChannels != result.blockAlign) {
                result.iStream.close();
                throw std::invalid_argument("Block Align does not agree with bytes required for validBits and number of channels");
            }

            numChunkBytes -= 16;
            
            if (numChunkBytes > 0) {
                result.iStream.close();
                result.iStream.ignore(numChunkBytes);
            }

        } else if(chunkID == DATA_CHUNK_ID) {
            if (foundFormat == false) {
                result.iStream.close();
                throw std::invalid_argument("Data chunk found before Format chunk");
            }

            if (chunkSize % result.blockAlign != 0) {
                result.iStream.close();
                throw std::invalid_argument("Data Chunk size is not multiple of Block Align");
            }

            result.numFrames = chunkSize / result.blockAlign;
            foundData = true;
            break;
        } else {
            result.iStream.ignore(numChunkBytes);
        }
    }

    if (foundData == false) {
        result.iStream.close();
        throw std::invalid_argument("Did not find a data chunk");
    }

    if (result.validBits > 8) {
        result.floatOffset = 0;
        result.floatScale = 1 << (result.validBits - 1);
    } else {
        result.floatOffset = -1;
        result.floatScale = 0.5 * ((1 << result.validBits) - 1);
    }

    result.bufferPointer = 0;
    result.bytesRead = 0;
    result.frameCounter = 0;
    return result;
}

int WavFile::getLE(vector<char>& buffer, int pos, int numBytes)
{
	numBytes--;
	pos += numBytes;
	int val = buffer[pos] & 255;
	for (auto b = 0; b < numBytes; b++)
		val = (val << 8) + (buffer[--pos] & 255);
	return val;
}

void WavFile::putLE(int val, vector<char>& buffer, int pos, int numBytes)
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
			oStream.write(&buffer[0], BUFFER_SIZE);
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
            
            iStream.read(&buffer[0], BUFFER_SIZE);
            auto read = iStream.gcount();

			if (read == 0)
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

int WavFile::readFrames(vector<float>* sampleBuffer, int numFramesToRead) 
{
    return readFrames(sampleBuffer, 0, numFramesToRead);
}

int WavFile::readFrames(vector<float>* sampleBuffer, int offset, int numFramesToRead)
{
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
    if (iStream.is_open()) {
        iStream.close();
    }
    
    if (oStream.is_open()) {
        if (bufferPointer > 0) {
            oStream.write(&buffer[0], bufferPointer);
        }

        if (wordAlignAdjust) {
            oStream << 0x00;
        }

        oStream.close();
    }
}
