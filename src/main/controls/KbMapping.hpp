#pragma once

#include "mpc_fs.hpp"

#include <vector>
#include <utility>
#include <vector>
#include <string>

#include "KeyCodeHelper.hpp"
#include "hardware/ComponentId.h"

namespace mpc {
    class Mpc;
}

namespace mpc::controls
{
class KbMapping
{
    
public:
    explicit KbMapping(const fs::path configDirectory);

    void initializeDefaults();
    const VmpcKeyCode getKeyCodeFromLabel(const std::string& label);
    hardware::ComponentId getHardwareComponentIdAssociatedWithKeycode(const VmpcKeyCode keyCode);
    void setKeyCodeForLabel(const VmpcKeyCode keyCode, const std::string& label);

    void exportMapping();
    void importMapping();
    std::vector<std::pair<hardware::ComponentId, VmpcKeyCode>>& getLabelKeyMap();

    static const VmpcKeyCode getNextKeyCode(const VmpcKeyCode keyCode);
    static const VmpcKeyCode getPreviousKeyCode(const VmpcKeyCode keyCode);

private:
    std::vector<std::pair<hardware::ComponentId, VmpcKeyCode>> idKeyMap;

    const fs::path configDirectory;

    const std::string magicHeader { "vmpc2000xl_keyboard_mapping" };
};
}
