#pragma once

namespace ctoot::mpc {

    class IndivFxMixer
    {

    private:
        int output{0};
        int volumeIndividualOut{0};
        int fxPath{0};
        int fxSendLevel{0};
        bool followStereo{false};

    public:
        void setFollowStereo(bool b);

        bool isFollowingStereo();

        int getOutput();

        void setOutput(int i);

        void setVolumeIndividualOut(int i);

        int getVolumeIndividualOut();

        void setFxPath(int i);

        int getFxPath();

        void setFxSendLevel(int i);

        int getFxSendLevel();

    public:
        IndivFxMixer();

        virtual ~IndivFxMixer()
        {}

    };
}
