#pragma once

namespace mpc::engine
{

    class StereoMixer
    {

    private:
        int panning = 0;
        int level = 0;

    public:
        void setPanning(int i);
        int getPanning();
        void setLevel(int i);
        int getLevel();

    public:
        StereoMixer();
    };
} // namespace mpc::engine
