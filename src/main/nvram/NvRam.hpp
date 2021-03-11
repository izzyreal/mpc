#pragma once

namespace mpc { class Mpc; }

namespace mpc::nvram
{
class NvRam
{
private:
    static const int DEFAULT_REC_GAIN = 20;
    static const int DEFAULT_MAIN_VOLUME = 65;
    
public:
    static void loadUserScreenValues(mpc::Mpc& mpc);
    static void saveUserScreenValues(mpc::Mpc& mpc);
    
    static void saveVmpcSettings(mpc::Mpc& mpc);
    static void loadVmpcSettings(mpc::Mpc& mpc);
    
};
}
