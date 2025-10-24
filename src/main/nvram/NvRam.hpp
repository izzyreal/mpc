#pragma once

namespace mpc
{
    class Mpc;
}

namespace mpc::nvram
{
    class NvRam
    {
    private:
        static const int DEFAULT_REC_GAIN = 20;
        static const int DEFAULT_MAIN_VOLUME = 65;

    public:
        static void loadUserScreenValues(mpc::Mpc &);
        static void saveUserScreenValues(mpc::Mpc &);

        static void loadVmpcSettings(mpc::Mpc &);
        static void saveVmpcSettings(mpc::Mpc &);
    };
} // namespace mpc::nvram
