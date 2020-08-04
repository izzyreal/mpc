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
	setSize(248, 60);
	setLocation(0, 0);
}

void Wave::setFine(bool fine)
{
	this->fine = fine;

	if (fine)
	{
		width = 109;
	}
	else
	{
		width = 245;
	}
}

void Wave::zoomPlus()
{
	if (zoomFactor == 7)
	{
		return;
	}

	zoomFactor++;
	
	initSamplesPerPixel();
	SetDirty();
}

void Wave::zoomMinus()
{
	if (zoomFactor == 1)
	{
		return;
	}

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
		{
			samplesPerPixel *= 2;
		}
	}
	else
	{
		samplesPerPixel = (float)frameCount / (float)width;
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
	{
		return;
	}

	selectionStart = start;
	selectionEnd = end;
	SetDirty();
}

void Wave::makeLine(std::vector<std::vector<std::vector<int>>>* lines, std::vector<bool>* colors, unsigned int x)
{
	int offset = 0;
	float peakPos = 0;
	float peakNeg = 0;
	int centerSamplePixel = 0;
	
	if (fine)
	{
		//offset += centerSamplePos;
		centerSamplePixel = centerSamplePos / samplesPerPixel;
	}
	
	int samplePos = (int)(floor((float) (x - (fine ? (54 - centerSamplePixel) : 0)) * samplesPerPixel));
	offset += samplePos;
	
	if (!mono && view == 1)
	{
		offset += frameCount;
	}

	if (offset < 0 || offset >= sampleData->size() && !fine)
	{
		return;
	}
	
	if (!mono && view == 0 && offset > frameCount && !fine)
	{
		return;
	}

	if (!mono && view == 1 && offset < frameCount && !fine)
	{
		return;
	}

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
	const float invisible = 1214.0 / 32768.0;
	const float ratio = 1.0f / (1.0f - invisible);

	const unsigned int posLineLength = (unsigned int) (floor(13.0 * ((peakPos - invisible) * ratio)));
	const unsigned int negLineLength = (unsigned int)(floor(13.0 * ((abs(peakNeg) - invisible) * ratio)));

	if (posLineLength != 13)
	{
		lines->push_back(Bressenham::Line(x, 0, x, 13 - (posLineLength + 1)));
	}

	if (peakPos > invisible)
	{
		lines->push_back(Bressenham::Line(x, (13 - posLineLength) - 1, x, 12));
	}

	if (abs(peakNeg) > invisible)
	{
		lines->push_back(Bressenham::Line(x, 13, x, 13 + negLineLength));
	}

	if (negLineLength != 13)
	{
		lines->push_back(Bressenham::Line(x, 13 + (negLineLength + 1), x, 26));
	}

	colors->clear();

	if (samplePos >= selectionStart && samplePos + samplesPerPixel < selectionEnd && !fine)
	{
		if (posLineLength != 13)
		{
			colors->push_back(true);
		}
		
		if (peakPos > invisible)
		{
			colors->push_back(false);
		}
		
		if (abs(peakNeg) > invisible)
		{
			colors->push_back(false);
		}
	
		if (negLineLength != 13)
		{
			colors->push_back(true);
		}
	}
	else
	{
		if (posLineLength != 13)
		{
			colors->push_back(false);
		}
		
		if (peakPos > invisible)
		{
			if (fine)
			{
				if (samplePos + samplesPerPixel >= frameCount)
				{
					colors->push_back(false);
				}
				else
				{
					colors->push_back(true);
				}
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
				if (samplePos + samplesPerPixel >= frameCount)
				{
					colors->push_back(false);
				}
				else
				{
					colors->push_back(true);
				}
			}
			else
			{
				colors->push_back(true);
			}
		}

		if (negLineLength != 13)
		{
			colors->push_back(false);
		}
	}
}

void Wave::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	if (sampleData == nullptr)
	{
		return;
	}

	vector<vector<vector<int>>> lines;
	vector<bool> colors;
	vector<int> offsetxy{ fine ? 23 : 1 , fine ? 16 : 21 };

	for (int i = 0; i < width; i++)
	{
		if (i == 55 && fine)
		{
			for (int j = 0; j < 27; j++)
			{
				(*pixels)[i + offsetxy[0]][j + offsetxy[1]] = true;
			}
			continue;
		}

		makeLine(&lines, &colors, i);
		int counter = 0;
		
		for (auto& l : lines)
		{
			mpc::Util::drawLine(*pixels, l, colors[counter++], offsetxy);
		}
	}

	dirty = false;
}
