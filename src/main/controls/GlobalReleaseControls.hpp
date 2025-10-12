#pragma once

namespace mpc { class Mpc; }

namespace mpc::controls {
    class PadReleaseContext;

    class GlobalReleaseControls
    {

    public:
        static void simplePad(PadReleaseContext&);

        static void overDub(mpc::Mpc&);

        static void rec(mpc::Mpc&);

        static void play(mpc::Mpc&);

        static void tap(mpc::Mpc&);

        static void shift(mpc::Mpc&);

        static void erase(mpc::Mpc&);

        static void function(mpc::Mpc&, const int i);

        static void goTo(mpc::Mpc&);
    };
}
