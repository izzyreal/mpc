#include "OthersScreen.hpp"

using namespace mpc::lcdgui::screens;

OthersScreen::OthersScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "others", layerIndex)
{
}

void OthersScreen::open()
{
    displayTapAveraging();
}

void OthersScreen::displayTapAveraging() const
{
    findField("tapaveraging")->setText(std::to_string(tapAveraging));
}

void OthersScreen::setTapAveraging(int i)
{
    if (i < 2 || i > 4)
    {
        return;
    }
    tapAveraging = i;
    displayTapAveraging();
}

int OthersScreen::getTapAveraging() const
{
    return tapAveraging;
}

void OthersScreen::setContrast(int i)
{
    if (i < 0 || i > 50)
    {
        return;
    }
    contrast = i;
    notifyObservers(std::string("contrast"));
}

int OthersScreen::getContrast() const
{
    return contrast;
}

void OthersScreen::function(int i)
{
    switch (i)
    {
        case 1:
            openScreenById(ScreenId::InitScreen);
            break;
        case 2:
            openScreenById(ScreenId::VerScreen);
            break;
    }
}

void OthersScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (false /*mpc.getControls()->isAltPressed()*/)
    {
        auto increment = i > 0 ? 1 : -1;
        setContrast(contrast + increment);
    }
    if (focusedFieldName == "tapaveraging")
    {
        setTapAveraging(tapAveraging + i);
    }
}
