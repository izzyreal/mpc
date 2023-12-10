#pragma once

#include <cmath>

namespace mpc::sequencer {

    class Clock
    {

    private:
        static const double ppqn_f;
        static const double rec60;
        static const double rec03;
        double bpm, omega, pd;
        double vpd, lvpd, dinphase;
        int tickN; // the tick number
        double Fs_rec;

    public:
        Clock();
        void init(double Fs);

        void set_bpm(const double &BPM);

        double getSampleRate() const
        { return 1.0 / Fs_rec; }

        bool proc();

        void reset();

        void zero();

        double getBpm() const;
    };

}
