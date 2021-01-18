#ifndef KeyEventHandler_h
#define KeyEventHandler_h

#include <memory>

namespace mpc::controls {
class KeyEvent;
}

namespace mpc::hardware {
class Hardware;
}

namespace mpc::controls {

class KeyEventHandler {
  
public:
    KeyEventHandler(std::weak_ptr<mpc::hardware::Hardware> hardware);
    void handle(const KeyEvent& keyEvent);
    
private:
    std::weak_ptr<mpc::hardware::Hardware> hardware;
    
};

}

#endif /* KeyEventHandler_h */
