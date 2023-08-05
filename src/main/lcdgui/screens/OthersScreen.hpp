#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <Observer.hpp>

namespace mpc::lcdgui::screens {

class OthersScreen
: public mpc::lcdgui::ScreenComponent, public Observable
{
  
private:
  void displayTapAveraging();
  int tapAveraging = 2;
  int contrast = 0;
  
public:
  void setTapAveraging(int i);
  int getTapAveraging();
  void setContrast(int i);
  int getContrast();
  
  OthersScreen(mpc::Mpc& mpc, const int layerIndex);
  void open() override;
  void function(int i) override;
  void turnWheel(int notch) override;
  
};
}
