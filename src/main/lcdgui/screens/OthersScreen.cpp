#include "OthersScreen.hpp"

#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"

using namespace mpc::lcdgui::screens;

OthersScreen::OthersScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "others", layerIndex)
{
}

void OthersScreen::open()
{
    displayTapAveraging();
    displayContrast();
}

void OthersScreen::displayTapAveraging() const
{
    findField("tapaveraging")->setText(std::to_string(tapAveraging));
}

void OthersScreen::displayContrast() const
{
    findField("contrast")->setText(std::to_string(contrast));
}

void OthersScreen::setTapAveraging(const int i)
{
    tapAveraging = std::clamp(i, 2, 4);
    displayTapAveraging();
}

int OthersScreen::getTapAveraging() const
{
    return tapAveraging;
}

void OthersScreen::setContrast(const int i)
{
    contrast = std::clamp(i, 0, 50);
    displayContrast();
    notifyObservers(std::string("contrast"));
}

int OthersScreen::getContrast() const
{
    return contrast;
}

void OthersScreen::function(const int i)
{
    switch (i)
    {
        case 1:
            openScreenById(ScreenId::InitScreen);
            break;
        case 2:
            openScreenById(ScreenId::VerScreen);
            break;
        default:;
    }
}

void OthersScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "contrast")
    {
        setContrast(contrast + i);
    }
    else if (focusedFieldName == "tapaveraging")
    {
        setTapAveraging(tapAveraging + i);
    }
}
