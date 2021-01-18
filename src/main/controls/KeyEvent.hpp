#ifndef KeyEvent_h
#define KeyEvent_h

namespace mpc::controls {

class KeyEvent {
  
public:
    KeyEvent(int rawKeyCode, bool keyDown);
    const int rawKeyCode;
    const bool keyDown;
    
};

}

#endif /* KeyEvent_h */
