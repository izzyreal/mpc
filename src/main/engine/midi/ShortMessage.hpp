#pragma once
#include "MidiMessage.hpp"

#include <vector>

namespace mpc::engine::midi {
class ShortMessage final
: public MidiMessage
{
    
public:
    static const int CHANNEL_PRESSURE{ 208 };
    static const int CONTINUE{ 251 };
    static const int CONTROL_CHANGE{ 176 };
    static const int NOTE_OFF{ 128 };
    static const int NOTE_ON{ 144 };
    static const int POLY_PRESSURE{ 160 };
    static const int START{ 250 };
    static const int STOP{ 252 };
    static const int TIMING_CLOCK{ 248 };

public:
    virtual int getChannel();
    virtual int getCommand();
    virtual int getData1();
    virtual int getData2();
    
public:
    int getDataLength(int status);
    
public:
    void setMessage(int status);
    void setMessage(int status, int data1, int data2);
    void setMessage(int command, int channel, int data1, int data2);

    ShortMessage();
    explicit ShortMessage(const std::vector<char>& data);
    
public:
    void setMessage(const std::vector<char>& data, int length) override;
    
};
}
