#pragma once
#include "Component.hpp"
#include <Observer.hpp>

#include "Screens.hpp"
#include "Field.hpp"
#include "Background.hpp"
#include "Wave.hpp"
#include "EnvGraph.hpp"

#include <memory>
#include <string>
#include <map>

namespace mpc::sampler
{
    class Sampler;
    class Program;
} // namespace mpc::sampler
namespace mpc::sequencer
{
    class Sequencer;
    class Track;
    class Bus;
    class DrumBus;
} // namespace mpc::sequencer
namespace mpc::lcdgui
{
    class LayeredScreen;
}

namespace mpc::lcdgui
{
    class ScreenComponent : public Component, public Observer
    {

    private:
        const int layer;
        std::map<std::string, std::vector<std::string>> transferMap;
        std::string firstField;

    public:
        const int &getLayerIndex();

        std::shared_ptr<sequencer::Bus> getBus();
        std::optional<int> getDrumIndex();
        std::string getFocusedFieldNameOrThrow();
        bool isFocusedFieldName(const std::string);
        std::shared_ptr<sampler::Program> getProgram();

    protected:
        mpc::Mpc &mpc;
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::lcdgui::LayeredScreen> ls;

        std::shared_ptr<mpc::sequencer::DrumBus> getActiveDrumBus();
        std::shared_ptr<Field> getFocusedField();
        std::shared_ptr<Field> getFocusedFieldOrThrow();
        std::optional<std::string> getFocusedFieldName();

        std::shared_ptr<sampler::Program> getProgramOrThrow();

    protected:
        void setLastFocus(const std::string &screenName,
                          const std::string &newLastFocus);
        const std::string getLastFocus(const std::string &screenName);
        std::shared_ptr<Wave> findWave();
        std::shared_ptr<EnvGraph> findEnvGraph();
        void openScreenById(const ScreenId);

    public:
        virtual void open() {}
        virtual void close() {}

    public:
        ScreenComponent(mpc::Mpc &mpc, const std::string &name,
                        const int layer);
        std::shared_ptr<Field> findFocus();
        void
        setTransferMap(const std::map<std::string, std::vector<std::string>>
                           &newTransferMap);
        void setFirstField(const std::string &newFirstField);
        std::string getFirstField();
        std::map<std::string, std::vector<std::string>> &getTransferMap();

    public:
        virtual void left();
        virtual void right();
        virtual void up();
        virtual void down();

        virtual void function(int i);

        virtual void openWindow();
        virtual void turnWheel(int) {}
        virtual void numpad(int i);
        virtual void pressEnter();
        virtual void rec();
        virtual void overDub();
        virtual void stop();
        virtual void play();
        virtual void playStart();
        virtual void setSlider(int) {}

        int getSoundIncrement(const int dataWheelSteps);
    };
} // namespace mpc::lcdgui
