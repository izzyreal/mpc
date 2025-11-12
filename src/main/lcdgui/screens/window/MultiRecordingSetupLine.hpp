#pragma once

namespace mpc::lcdgui::screens::window
{
    class MultiRecordingSetupLine
    {

        int in = 0;
        int track = 0;

    public:
        void setIn(int in);
        int getIn() const;
        void setTrack(int i);
        int getTrack() const;
    };
} // namespace mpc::lcdgui::screens::window
