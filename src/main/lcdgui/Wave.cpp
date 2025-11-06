#include "Wave.hpp"

#include "Util.hpp"

#include "Bressenham.hpp"

using namespace mpc::lcdgui;

Wave::Wave() : Component("wave")
{
    Component::setSize(246, 27);
    setLocation(1, 21);
}

void Wave::setFine(const bool newFineEnabled)
{
    fine = newFineEnabled;
    setSize(newFineEnabled ? 109 : 246, 27);
    setLocation(newFineEnabled ? 23 : 1, newFineEnabled ? 16 : 21);
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
        samplesPerPixel =
            static_cast<float>(frameCount) / static_cast<float>(w);

        if (samplesPerPixel < 1)
        {
            samplesPerPixel = 1;
        }
    }
}

void Wave::setCenterSamplePos(const unsigned int newCenterSamplePos)
{
    centerSamplePos = newCenterSamplePos;
    SetDirty();
}

void Wave::setSampleData(
    const std::shared_ptr<const std::vector<float>> &newSampleData,
    const bool newMono, const unsigned int newView)
{
    const auto newFrameCount =
        newSampleData != nullptr
            ? static_cast<int>(floor(newMono ? newSampleData->size()
                                             : (newSampleData->size() * 0.5)))
            : 0;

    if (sampleData == newSampleData && newFrameCount == frameCount &&
        mono == newMono && view == newView)
    {
        return;
    }

    this->sampleData = newSampleData;

    if (newSampleData == nullptr)
    {
        frameCount = 0;
        return;
    }

    this->mono = newMono;
    this->view = newView;

    frameCount = newFrameCount;

    initSamplesPerPixel();
    SetDirty();
}

void Wave::setSelection(const unsigned int start, const unsigned int end)
{
    if (selectionStart == start && selectionEnd == end)
    {
        return;
    }

    selectionStart = start;
    selectionEnd = end;

    if (selectionEnd - selectionStart < (samplesPerPixel * 2))
    {
        selectionEnd = selectionStart + (samplesPerPixel * 2);
    }

    SetDirty();
}

void Wave::makeLine(LcdBitmap &bitmap, std::vector<bool> *colors,
                    const unsigned int lineX) const
{
    int offset = 0;
    float peakPos = 0;
    float peakNeg = 0;
    double centerSamplePixel = 0.f;

    if (fine)
    {
        centerSamplePixel = (centerSamplePos / samplesPerPixel) - 1;
    }

    const int samplePos = static_cast<int>(floor(
        static_cast<float>(lineX - (fine ? (54 - centerSamplePixel) : 0)) *
        samplesPerPixel));
    offset += samplePos;

    if (!mono && view == 1)
    {
        offset += frameCount;
    }

    if (offset < 0 || (mono && offset >= frameCount && !fine))
    {
        return;
    }

    if (!mono && view == 0 && offset >= frameCount && !fine)
    {
        return;
    }

    if (!mono && view == 1 && offset < frameCount && !fine)
    {
        return;
    }

    for (int i = 0; i < (floor)(samplesPerPixel); i++)
    {

        if (const float sample = (*sampleData)[offset++]; sample > 0)
        {
            peakPos = std::fmax(peakPos, sample);
        }
        else if (sample < 0)
        {
            peakNeg = std::fmin(peakNeg, sample);
        }
    }

    bitmap.clear();
    colors->clear();

    if (fine && lineX == 55)
    {
        bitmap.emplace_back(Bressenham::Line(lineX, 0, lineX, 26));
        colors->push_back(true);
    }

    constexpr float invisible = 1214.0 / 32768.0;
    constexpr float ratio = 1.0f / (1.0f - invisible);

    const unsigned int posLineLength = static_cast<unsigned int>(
        floor(13.0 * ((peakPos - invisible) * ratio)));
    const unsigned int negLineLength = static_cast<unsigned int>(
        floor(13.0 * ((std::fabs(peakNeg) - invisible) * ratio)));

    if (posLineLength != 13 && !(fine && lineX == 55))
    {
        bitmap.emplace_back(
            Bressenham::Line(lineX, 0, lineX, 13 - (posLineLength + 1)));
    }

    if (peakPos > invisible)
    {
        bitmap.emplace_back(
            Bressenham::Line(lineX, (13 - posLineLength) - 1, lineX, 12));
    }

    if (std::fabs(peakNeg) > invisible)
    {
        bitmap.emplace_back(
            Bressenham::Line(lineX, 13, lineX, 13 + negLineLength));
    }

    if (negLineLength != 13 && !(fine && lineX == 55))
    {
        bitmap.emplace_back(
            Bressenham::Line(lineX, 13 + (negLineLength + 1), lineX, 26));
    }

    if (!fine && samplePos >= selectionStart &&
        samplePos + samplesPerPixel < selectionEnd)
    {
        if (posLineLength != 13)
        {
            colors->push_back(true);
        }

        if (peakPos > invisible)
        {
            colors->push_back(false);
        }

        if (std::fabs(peakNeg) > invisible)
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
        if (posLineLength != 13 && !(fine && lineX == 55))
        {
            colors->push_back(false);
        }

        if (peakPos > invisible)
        {
            if (fine)
            {
                if (static_cast<int>(floor(samplePos + samplesPerPixel)) >=
                        frameCount ||
                    lineX == 55)
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

        if (std::fabs(peakNeg) > invisible)
        {
            if (fine)
            {
                if (static_cast<int>(floor(samplePos + samplesPerPixel)) >=
                        frameCount ||
                    lineX == 55)
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

        if (negLineLength != 13 && !(fine && lineX == 55))
        {
            colors->push_back(false);
        }
    }
}

void Wave::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    if (sampleData == nullptr)
    {
        return;
    }

    Clear(pixels);

    LcdBitmap bitmap;
    std::vector<bool> colors;

    for (int i = 0; i < w; i++)
    {
        makeLine(bitmap, &colors, i);
        int counter = 0;

        for (auto &line : bitmap)
        {
            mpc::Util::drawLine(*pixels, line, colors[counter++],
                                std::vector<int>{x, y});
        }
    }

    dirty = false;
}
