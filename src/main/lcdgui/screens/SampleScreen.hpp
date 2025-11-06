#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <map>

namespace mpc::audiomidi
{
    class SoundRecorder;
}

namespace mpc::lcdgui::screens
{
    class SampleScreen final : public ScreenComponent
    {
    public:
        SampleScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void function(int i) override;
        void turnWheel(int i) override;
        void openWindow() override;
        void left() override;
        void right() override;
        void up() override;
        void down() override;

        void setCurrentBufferPeak(const std::pair<float, float> &);

    private:
        std::map<int, int> vuPosToDb{
            {0, -63},  {1, -59},  {2, -55},  {3, -51},  {4, -47},  {5, -43},
            {6, -39},  {7, -35},  {8, -34},  {9, -33},  {10, -31}, {11, -29},
            {12, -27}, {13, -25}, {14, -23}, {15, -21}, {16, -19}, {17, -17},
            {18, -15}, {19, -14}, {20, -13}, {21, -12}, {22, -11}, {23, -10},
            {24, -9},  {25, -8},  {26, -7},  {27, -6},  {28, -5},  {29, -4},
            {30, -3},  {31, -2},  {32, -1},  {33, 0}};
        int input = 0;
        int threshold = -20;
        int mode = 2;
        int time = 100;
        int monitor = 0;
        int preRec = 100;
        std::pair<float, float> peak;
        std::atomic<float> currentBufferPeakL;
        std::atomic<float> currentBufferPeakR;

        void setInput(int i);
        void setThreshold(int i);
        void setMode(int i);
        void setTime(int i);
        void setMonitor(int i);
        void setPreRec(int i);

        void displayInput();
        void displayThreshold();
        void displayMode();
        void displayTime();
        void displayMonitor();
        void displayPreRec();
        void updateVU();

    public:
        int getMode() const;
        int getMonitor() const;

    private:
        const std::string vu_normal = u8"\u00F5";
        const std::string vu_threshold = u8"\u00F6";
        const std::string vu_peak = u8"\u00F8";
        const std::string vu_peak_threshold = u8"\u00F9";
        const std::string vu_normal_threshold = u8"\u00FA";
        const std::string vu_peak_threshold_normal = u8"\u00FB";

        std::vector<std::string> inputNames{"ANALOG", "DIGITAL"};
        std::vector<std::string> modeNames{"MONO L", "MONO R", "STEREO"};
        std::vector<std::string> monitorNames{"OFF", "L/R", "1/2",
                                              "3/4", "5/6", "7/8"};

        friend class audiomidi::SoundRecorder;
    };
} // namespace mpc::lcdgui::screens
