#pragma once
#include "Component.hpp"

#include "mpc_types.hpp"

#include <vector>

namespace mpc::lcdgui
{
    class Wave : public Component
    {

    private:
        std::shared_ptr<const std::vector<float>> sampleData;
        bool fine{false};
        bool mono{true};
        unsigned int view = 0;
        float samplesPerPixel{1.0f};
        unsigned int frameCount = 0;
        float exp{1.0f};
        unsigned int selectionStart = 0;  // normal view only
        unsigned int selectionEnd = 0;    // normal view only
        unsigned int centerSamplePos = 0; // zoomed view only
        unsigned int zoomFactor{3};       // zoomed view only

    private:
        void makeLine(LcdBitmap &, std::vector<bool> *colors,
                      unsigned int samplePos);
        void initSamplesPerPixel();

    public:
        void
        setSampleData(std::shared_ptr<const std::vector<float>> newSampleData,
                      bool newMono, unsigned int newView);
        void setSelection(unsigned int start, unsigned int end);

    public:
        void setFine(bool newFineEnabled);
        void zoomPlus();
        void zoomMinus();
        void setCenterSamplePos(unsigned int newCenterSamplePos);

    public:
        void Draw(std::vector<std::vector<bool>> *pixels) override;

    public:
        Wave();
    };
} // namespace mpc::lcdgui
