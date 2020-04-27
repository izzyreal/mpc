#pragma once

#include <vector>
#include <memory>

#include <fstream>

using namespace std;

namespace mpc::file::wav {

	class WavFile
	{
	public:
		static WavFile newWavFile(const string& path, int numChannels, int numFrames, int validBits, int sampleRate);
		static WavFile openWavFile(const string& path);

	private:
		vector<char> buffer;
		ifstream iStream;
		ofstream oStream;

	private:
		static const int BUFFER_SIZE{ 2048 };
		static const int FMT_CHUNK_ID{ 544501094 };
		static const int DATA_CHUNK_ID{ 1635017060 };
		static const int RIFF_CHUNK_ID{ 1179011410 };
		static const int RIFF_TYPE_ID{ 1163280727 };
		int bytesPerSample{};
		int numFrames{};
		double floatScale{};
		double floatOffset{};
		bool wordAlignAdjust{};
		int numChannels{};
		int sampleRate{};
		int blockAlign{};
		int validBits{};
		int bufferPointer{};
		int bytesRead{};
		int frameCounter{};

	public:
		int getNumChannels();
		int getNumFrames();
		int getFramesRemaining();
		int getSampleRate();
		int getValidBits();

	private:
		static int getLE(vector<char>& buffer, int pos, int numBytes);
		static void putLE(int val, vector<char>& buffer, int pos, int numBytes);
		void writeSample(int val);
		int readSample();

	public:
		int readFrames(vector<int>* sampleBuffer, int numFramesToRead);
		int readFrames(vector<int>* sampleBuffer, int offset, int numFramesToRead);
		int readFrames(vector<vector<int>>* sampleBuffer, int numFramesToRead);
		int readFrames(vector<vector<int>>* sampleBuffer, int offset, int numFramesToRead);
		int writeFrames(vector<int>* sampleBuffer, int numFramesToWrite);
		int writeFrames(vector<int>* sampleBuffer, int offset, int numFramesToWrite);
		int writeFrames(vector<vector<int>>* sampleBuffer, int numFramesToWrite);
		int writeFrames(vector<vector<int>>* sampleBuffer, int offset, int numFramesToWrite);
		int readFrames(vector<float>* sampleBuffer, int numFramesToRead);
		int readFrames(vector<float>* sampleBuffer, int offset, int numFramesToRead);
		int readFrames(vector<vector<float>>* sampleBuffer, int numFramesToRead);
		int readFrames(vector<vector<float>>* sampleBuffer, int offset, int numFramesToRead);
		int writeFrames(vector<float>* sampleBuffer, int numFramesToWrite);
		int writeFrames(vector<float>* sampleBuffer, int offset, int numFramesToWrite);
		int writeFrames(vector<vector<float>>* sampleBuffer, int numFramesToWrite);
		int writeFrames(vector<vector<float>>* sampleBuffer, int offset, int numFramesToWrite);
		void close();

	public:
		WavFile();

	};
}
