#pragma once
#include <observer/Observable.hpp>
#include <Mpc.hpp>

#include <memory>

namespace mpc { class Mpc; }

namespace mpc::sequencer {
class Track;
class TimeSignature;
class TempoChangeEvent;
}

namespace mpc::sequencer {

class Sequence final
: public moduru::observer::Observable
{
    
private:
    mpc::Mpc& mpc;
    double initialTempo = 120.0;
    
    std::vector<std::shared_ptr<Track>> tracks;
    std::vector<std::shared_ptr<Track>> metaTracks;

    std::vector<std::string> deviceNames = std::vector<std::string>(33);
    std::vector<std::string> defaultTrackNames;
    
    std::vector<int> barLengthsInTicks = std::vector<int>(999);
    std::vector<int> numerators = std::vector<int>(999);
    std::vector<int> denominators = std::vector<int>(999);
    
    std::string name = "";
    bool loopEnabled = true;
    int lastBarIndex = -1;
    bool used = false;
    bool tempoChangeOn = true;
    int loopStart = 0;
    int loopEnd = 0;
    int firstLoopBarIndex = 0;
    int lastLoopBarIndex = 0;
    bool lastLoopBarEnd = true;
    
public:
    double getInitialTempo();
    void setInitialTempo(const double newInitialTempo);
    
    void setLoopStart(int l);
    int getLoopStart();
    void setLoopEnd(int l);
    int getLoopEnd();
    void setFirstLoopBarIndex(int i);
    int getFirstLoopBarIndex();
    void setLastLoopBarIndex(int i);
    int getLastLoopBarIndex();
    void initMetaTracks();
    
public:
    void createMidiClockTrack();
    void createTempoChangeTrack();
    
    static bool trackIndexComparator(std::shared_ptr<Track>& t0, std::shared_ptr<Track>& t1);
    
public:
    bool isLoopEnabled();
    void setName(std::string s);
    std::string getName();
    void setDeviceName(int i, std::string s);
    std::string getDeviceName(int i);
    void setLastBar(int i);
    int getLastBarIndex();
    void setLoopEnabled(bool b);
    std::shared_ptr<Track> getTrack(int i);
    void setUsed(bool b);
    bool isUsed();
    void init(int newLastBarIndex);
    void setTimeSignature(int firstBar, int tsLastBar, int num, int den);
    void setTimeSignature(int barIndex, int num, int den);
    std::vector<std::shared_ptr<Track>> getTracks();
    std::vector<std::shared_ptr<Track>> getMetaTracks();
    std::vector<std::string>& getDeviceNames();
    void setDeviceNames(std::vector<std::string>& sa);
    std::vector<std::shared_ptr<TempoChangeEvent>> getTempoChangeEvents();
    std::shared_ptr<TempoChangeEvent> addTempoChangeEvent();
    void removeTempoChangeEvent(int i);

    bool isTempoChangeOn();
    void setTempoChangeOn(bool b);
    int getLastTick();
    TimeSignature getTimeSignature();
    void sortTempoChangeEvents();
    void purgeAllTracks();
    std::shared_ptr<Track> purgeTrack(int i);
    int getDenominator(int i);
    int getNumerator(int i);
    std::vector<int>& getBarLengthsInTicks();
    void setBarLengths(std::vector<int>&);
    void setNumeratorsAndDenominators(std::vector<int>& numerators, std::vector<int>& denominators);
    void deleteBars(int firstBar, int lBar);
    void insertBars(int numberOfBars, int afterBar);
    void moveTrack(int source, int destination);
    bool isLastLoopBarEnd();
    
    int getEventCount();
    void initLoop();
    std::vector<int>& getNumerators();
    std::vector<int>& getDenominators();

    int getFirstTickOfBar(int index);
    int getLastTickOfBar(int index);
    int getFirstTickOfBeat(int bar, int beat);
    
    void resetTrackEventIndices(int tick);
    
public:
    Sequence(mpc::Mpc&);
    
};
}
