#include "KeyEvent.hpp"

using namespace mpc::controls;

KeyEvent::KeyEvent(int rawKeyCode, bool keyDown)
: rawKeyCode (rawKeyCode), keyDown (keyDown)
{
}
