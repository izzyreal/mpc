#pragma once

namespace mpc::engine
{

    class IndivFxMixer
    {

        int output{0};
        int volumeIndividualOut{0};
        int fxPath{0};
        int fxSendLevel{0};
        bool followStereo{false};

    public:
        void setFollowStereo(bool b);

        bool isFollowingStereo() const;

        int getOutput() const;

        void setOutput(int i);

        void setVolumeIndividualOut(int i);

        int getVolumeIndividualOut() const;

        void setFxPath(int i);

        int getFxPath() const;

        void setFxSendLevel(int i);

        int getFxSendLevel() const;

        IndivFxMixer();
    };
} // namespace mpc::engine
