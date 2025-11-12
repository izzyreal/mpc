#pragma once

namespace mpc
{
    class Mpc;
}

namespace mpc::nvram
{
    class NvRam
    {
        static const int DEFAULT_REC_GAIN = 20;
        static const int DEFAULT_MAIN_VOLUME = 65;

    public:
        static void loadUserScreenValues(Mpc &);
        static void saveUserScreenValues(Mpc &);

        static void loadVmpcSettings(Mpc &);
        static void saveVmpcSettings(Mpc &);
    };
} // namespace mpc::nvram
