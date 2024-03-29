#pragma once

namespace mpc::engine {
    class MuteInfo {

    private:
        int note{0};
        int drum{0};

    public:
        void setNote(int note);

        void setDrum(int drum);

        int getNote();

        int getDrum();

        bool shouldMute(int otherNote, int otherDrum);

        MuteInfo();

    };

}
