#pragma once
#include <vector>
#include <utility>
#include <vector>
#include <string>

#include "KeyCodeHelper.hpp"

namespace mpc {
    class Mpc;
}

namespace mpc::controls
{
class KbMapping
{
    
public:
    void initializeDefaults();
    const VmpcKeyCode getKeyCodeFromLabel(const std::string& label);
    std::string getHardwareComponentLabelAssociatedWithKeycode(const VmpcKeyCode keyCode);
    void setKeyCodeForLabel(const VmpcKeyCode keyCode, const std::string& label);

    void exportMapping();
    void importMapping();
    std::vector<std::pair<std::string, VmpcKeyCode>>& getLabelKeyMap();

    KbMapping(mpc::Mpc&);

    static const VmpcKeyCode getNextKeyCode(const VmpcKeyCode keyCode);
    static const VmpcKeyCode getPreviousKeyCode(const VmpcKeyCode keyCode);

private:
    mpc::Mpc& mpc;
    std::vector<std::pair<std::string, VmpcKeyCode>> labelKeyMap;

    // migrate persisted mappings in old format to new
    void migrateV0_4_4MappingToV0_5();
   
    const std::string magicHeader { "vmpc2000xl_keyboard_mapping" };
};
}
