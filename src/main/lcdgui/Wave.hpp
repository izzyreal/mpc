#pragma once
#include "Component.hpp"

#include <vector>

namespace mpc::lcdgui
{
	class Wave
		: public Component
	{

	private:
		std::vector<float>* sampleData{ nullptr };
		bool fine{ false };
		bool mono{ true };
		unsigned int view{ 0 };
		float samplesPerPixel{ 1.0f };
		unsigned int frameCount{ 0 };
		float exp{ 1.0f };
		unsigned int selectionStart{ 0 };	// normal view only
		unsigned int selectionEnd{ 0 };		// normal view only
		unsigned int centerSamplePos{ 0 };	// zoomed view only
		unsigned int zoomFactor{ 7 };		// zoomed view only
		unsigned int width{ 0 };

	private:
		void makeLine(std::vector<std::vector<std::vector<int>>>* lines, std::vector<bool>* colors, unsigned int samplePos);
		void initSamplesPerPixel();

	public:
		void setSampleData(std::vector<float>* sampleData, bool mono, unsigned int view);
		void setSelection(unsigned int start, unsigned int end);

	public:
		void setFine(bool fine);
		void zoomPlus();
		void zoomMinus();
		void setCenterSamplePos(unsigned int centerSamplePos);

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		Wave();

	};
}
