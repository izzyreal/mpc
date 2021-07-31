#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens {
class VmpcDisksScreen : public mpc::lcdgui::ScreenComponent
{
public:
    VmpcDisksScreen(mpc::Mpc& mpc, const int layerIndex);
    void open() override;
    void function(int i) override;
    void turnWheel(int i) override;
};
}
