#pragma once

namespace mpc::engine
{

    class StereoMixer
    {

        int panning = 0;
        int level = 0;

    public:
        void setPanning(int i);
        int getPanning() const;
        void setLevel(int i);
        int getLevel() const;

        StereoMixer();
    };
} // namespace mpc::engine
