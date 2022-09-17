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
    std::string getLabelFromKeyCode(int keyCode);
    static std::string getKeyCodeString(int keyCode);
    void setKeyCodeForLabel(const int keyCode, std::string label);
    std::vector<std::string> getMappedLabels();
    
    void exportMapping();
    void importMapping();
    
    KbMapping();
    
private:
    const static WonderRabbitProject::key::key_helper_t* kh;
    std::vector<std::pair<std::string, int>> labelKeyMap;
    
};
}
