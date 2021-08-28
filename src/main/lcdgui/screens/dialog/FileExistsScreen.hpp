#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
class FileExistsScreen
: public mpc::lcdgui::ScreenComponent
{
public:
    FileExistsScreen(mpc::Mpc& mpc, const int layerIndex);
    void function(int i) override;
};
}
