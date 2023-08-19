#pragma once

#include <vector>
#include <memory>

namespace mpc { class Mpc; }

namespace mpc::nvram
{
struct MidiControlPreset;
class NvRam
{
private:
    static const int DEFAULT_REC_GAIN = 20;
    static const int DEFAULT_MAIN_VOLUME = 65;
    
public:
    // Presets available in the default preset path,
    // loaded into memory, and maintained as presets are changed, added or deleted.
    static std::vector<std::shared_ptr<MidiControlPreset>> presets;

    static void loadUserScreenValues(mpc::Mpc&);
    static void saveUserScreenValues(mpc::Mpc&);

    static void loadVmpcSettings(mpc::Mpc&);
    static void saveVmpcSettings(mpc::Mpc&);
    
    
};
}
