#pragma once
#include <cstdint>
#include <vector>
#include <thirdp/wrpkey/key.hxx>

#include <utility>
#include <vector>
#include <string>

namespace mpc::controls
{
class KbMapping
{
    
public:
    void initializeDefaults();
    int getKeyCodeFromLabel(const std::string& label);
    std::string getHardwareComponentLabelAssociatedWithKeycode(int keyCode);
    static std::string getKeyCodeString(int keyCode);
    void setKeyCodeForLabel(const int keyCode, std::string label);

    void exportMapping();
    void importMapping();
    std::vector<std::pair<std::string, int>>& getLabelKeyMap();

    KbMapping();

    static int getNextKeyCode(int keyCode);
    static int getPreviousKeyCode(int keyCode);

private:
    const static WonderRabbitProject::key::key_helper_t* kh;
    std::vector<std::pair<std::string, int>> labelKeyMap;

    // migrate persisted mappings in old format to new
    void migrateV0_4_4MappingToV0_5();
    
};
}
