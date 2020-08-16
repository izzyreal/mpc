#include "Wave.hpp"

#include <Util.hpp>

#include <math/Math.hpp>

#include <gui/Bressenham.hpp>

#include <cmath>

using namespace mpc::lcdgui;
using namespace moduru::gui;
using namespace std;

Wave::Wave()
	: Component("wave")
{
	setSize(246, 27);
	setLocation(1, 21);
}

void Wave::setFine(bool fine)
{
	this->fine = fine;
	setSize(fine ? 109 : 246, 27);
	setLocation(fine ? 23 : 1, fine ? 16 : 21);
}

void Wave::zoomPlus()
{
	if (zoomFactor == 7)
		return;

	zoomFactor++;
	
	initSamplesPerPixel();
	SetDirty();
}

void Wave::zoomMinus()
{
	if (zoomFactor == 1)
		return;

	zoomFactor--;
	
	initSamplesPerPixel();
	SetDirty();
}

void Wave::initSamplesPerPixel()
{
	if (fine)
	{
		samplesPerPixel = 1;
		
		for (int i = 1; i < zoomFactor; i++)
			samplesPerPixel *= 2;
	}
	else
	{
		samplesPerPixel = (float)frameCount / (float)w;
	}
}

void Wave::setCenterSamplePos(unsigned int centerSamplePos)
{
	this->centerSamplePos = centerSamplePos;
	SetDirty();
}

void Wave::setSampleData(vector<float>* sampleData, bool mono, unsigned int view)
{
	auto newFrameCount = sampleData != nullptr ? (mono ? sampleData->size() : (sampleData->size() * 0.5)) : 0;

	if (this->sampleData == sampleData &&
		newFrameCount == frameCount &&
		this->mono == mono && this->view == view)
	{
		return;
	}

	this->sampleData = sampleData;

	if (sampleData == nullptr)
	{
		frameCount = 0;
		return;
	}

	this->mono = mono;
	this->view = view;
	
	frameCount = newFrameCount;
	
	initSamplesPerPixel();
	SetDirty();
}

void Wave::setSelection(unsigned int start, unsigned int end)
{
	if (selectionStart == start && selectionEnd == end)
		return;

	selectionStart = start;
	selectionEnd = end;

	if (selectionEnd - selectionStart < (samplesPerPixel * 2))
		selectionEnd = selectionStart + (samplesPerPixel * 2);

	SetDirty();
}

void Wave::makeLine(std::vector<std::vector<std::vector<int>>>* lines, std::vector<bool>* colors, unsigned int lineX)
{
	int offset = 0;
	float peakPos = 0;
	float peakNeg = 0;
	int centerSamplePixel = 0;
	
	if (fine)
		centerSamplePixel = (centerSamplePos / samplesPerPixel) - 1;
	
	int samplePos = (int)(floor((float) (lineX - (fine ? (54 - centerSamplePixel) : 0)) * samplesPerPixel));
	offset += samplePos;
	
	if (!mono && view == 1)
		offset += frameCount;

	if (offset < 0 || offset >= sampleData->size() && !fine)
		return;

	if (!mono && view == 0 && offset > frameCount && !fine)
		return;

	if (!mono && view == 1 && offset < frameCount && !fine)
		return;

	float sample;
	
	for (int i = 0; i < (floor)(samplesPerPixel); i++)
	{
		sample = (*sampleData)[offset++];
	
		if (sample > 0)
		{
			peakPos = moduru::math::Math::maxf(peakPos, sample);
		}
		else if (sample < 0) {
			peakNeg = moduru::math::Math::minf(peakNeg, sample);
		}
	}

	lines->clear();
	colors->clear();
	
	if (fine && lineX == 55)
	{
		lines->push_back(Bressenham::Line(lineX, 0, lineX, 26));
		colors->push_back(true);
	}

	const float invisible = 1214.0 / 32768.0;
	const float ratio = 1.0f / (1.0f - invisible);

	const unsigned int posLineLength = (unsigned int) (floor(13.0 * ((peakPos - invisible) * ratio)));
	const unsigned int negLineLength = (unsigned int)(floor(13.0 * ((abs(peakNeg) - invisible) * ratio)));

	if (posLineLength != 13 && !(fine && lineX == 55))
	{
		lines->push_back(Bressenham::Line(lineX, 0, lineX, 13 - (posLineLength + 1)));
	}

	if (peakPos > invisible)
	{
		lines->push_back(Bressenham::Line(lineX, (13 - posLineLength) - 1, lineX, 12));
	}

	if (abs(peakNeg) > invisible)
	{
		lines->push_back(Bressenham::Line(lineX, 13, lineX, 13 + negLineLength));
	}

	if (negLineLength != 13 && !(fine && lineX == 55))
	{
		lines->push_back(Bressenham::Line(lineX, 13 + (negLineLength + 1), lineX, 26));
	}

	if (!fine && samplePos >= selectionStart && samplePos + samplesPerPixel < selectionEnd)
	{
		if (posLineLength != 13)
			colors->push_back(true);
		
		if (peakPos > invisible)
			colors->push_back(false);
		
		if (abs(peakNeg) > invisible)
			colors->push_back(false);
	
		if (negLineLength != 13)
			colors->push_back(true);
	}
	else
	{
		if (posLineLength != 13 && !(fine && lineX == 55))
			colors->push_back(false);
		
		if (peakPos > invisible)
		{
			if (fine)
			{
				if (samplePos + samplesPerPixel >= frameCount || lineX == 55)
					colors->push_back(false);
				else
					colors->push_back(true);
			}
			else
			{
				colors->push_back(true);
			}
		}

		if (abs(peakNeg) > invisible)
		{
			if (fine)
			{
				if (samplePos + samplesPerPixel >= frameCount || lineX == 55)
					colors->push_back(false);
				else
					colors->push_back(true);
			}
			else
			{
				colors->push_back(true);
			}
		}

		if (negLineLength != 13 && !(fine && lineX == 55))
			colors->push_back(false);
	}
}

void Wave::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
		return;

	if (sampleData == nullptr)
		return;

	//if (fine)
		Clear(pixels);

	vector<vector<vector<int>>> lines;
	vector<bool> colors;

	for (int i = 0; i < w; i++)
	{
		makeLine(&lines, &colors, i);
		int counter = 0;
		
		for (auto& l : lines)
			mpc::Util::drawLine(*pixels, l, colors[counter++], vector<int>{x, y});
	}

	dirty = false;
}
