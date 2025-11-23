#include "InitPadAssignScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;

InitPadAssignScreen::InitPadAssignScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "init-pad-assign", layerIndex)
{
}

void InitPadAssignScreen::open()
{
    findField("init-pad-assign")->setAlignment(Alignment::Centered);
    displayInitPadAssign();
}

void InitPadAssignScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "init-pad-assign")
    {
        initPadAssignIsMaster = i > 0;
        displayInitPadAssign();
    }
}

void InitPadAssignScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
            if (initPadAssignIsMaster)
            {
                sampler.lock()->setMasterPadAssign(*sampler.lock()->getInitMasterPadAssign());
            }
            else
            {
                getProgramOrThrow()->initPadAssign();
            }

            openScreenById(ScreenId::PgmAssignScreen);
            break;
    }
}

void InitPadAssignScreen::displayInitPadAssign() const
{
    findField("init-pad-assign")
        ->setText(initPadAssignIsMaster ? "MASTER" : "PROGRAM");
}
