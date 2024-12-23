#pragma once
#include <vector>
#include <utility>
#include <vector>
#include <string>


namespace mpc {
    class Mpc;
}

namespace mpc::controls
{
class KbMapping
{
    
public:
    void initializeDefaults();
    int getKeyCodeFromLabel(const std::string& label);
    std::string getHardwareComponentLabelAssociatedWithKeycode(int keyCode);
    static std::string getKeyCodeString(int keyCode, const bool oneChar = false);
    void setKeyCodeForLabel(const int keyCode, const std::string& label);

    void exportMapping();
    void importMapping();
    std::vector<std::pair<std::string, int>>& getLabelKeyMap();

    KbMapping(mpc::Mpc&);

    static int getNextKeyCode(int keyCode);
    static int getPreviousKeyCode(int keyCode);

private:
    mpc::Mpc& mpc;
    std::vector<std::pair<std::string, int>> labelKeyMap;

    // migrate persisted mappings in old format to new
    void migrateV0_4_4MappingToV0_5();
    
};
}
