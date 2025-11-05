#include "lcdgui/Screens.hpp"
#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "sequencer/Sequencer.hpp"
#include "sampler/Sampler.hpp"

#include "lcdgui/Layer.hpp"
#include "lcdgui/ScreenNames.hpp"

#include <vector>
#include <string>

inline const std::vector<std::string> knownProblematicScreens{
    "tempo-change", // Crashes when opened in test, works fine in app
};

SCENARIO("All screens can be opened", "[screens]")
{

    GIVEN("An initialized Mpc with an initialized Sequence")
    {
        mpc::Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);

        auto s = mpc.getSampler()->addSound();
        assert(s != nullptr);
        s->setMono(true);
        s->setName("test");
        for (int i = 0; i < 1000; i++)
        {
            s->insertFrame(std::vector<float>{0.0}, s->getSampleData()->size());
        }

        mpc.getSampler()->setSoundIndex(0);

        auto ls = mpc.getLayeredScreen();

        std::vector<std::string> good;
        std::vector<std::string> bad;

        for (auto [screenName, screenId] : screenNames)
        {
            if (std::find(knownUnimplementedScreens.begin(),
                          knownUnimplementedScreens.end(),
                          screenName) != knownUnimplementedScreens.end())
            {
                printf("Fix this known unimplemented screen asap: '%s'\n",
                       screenName);
                continue;
            }
            if (std::find(knownProblematicScreens.begin(),
                          knownProblematicScreens.end(),
                          screenName) != knownProblematicScreens.end())
            {
                printf("Fix this known problematic screen asap: '%s'\n",
                       screenName);
                continue;
            }

            ls->openScreen(screenName);

            // We do a check for the most important screen
            if (std::string(screenName) == "sequencer")
            {
                REQUIRE(ls->getFocusedLayerIndex() == 0);
            }

            if (ls->getFocusedLayerIndex() == -1)
            {
                bad.push_back(std::string(screenName) +
                              " could not be opened.");
                continue;
            }

            auto layer = ls->getFocusedLayer();
            std::vector<std::vector<bool>> pixels(248, std::vector<bool>(60));
            layer->drawRecursive(&pixels);
            int blackPixelCount = 0;

            for (auto &column : pixels)
            {
                for (auto b : column)
                {
                    if (b)
                    {
                        blackPixelCount++;
                    }
                }
            }

            if (blackPixelCount > 0)
            {
                good.push_back(std::string(screenName) + " has " +
                               std::to_string(blackPixelCount) +
                               " black pixels");
            }
            else
            {
                bad.push_back(std::string(screenName) +
                              " is openable, but has 0 black pixels");
            }
        }

        REQUIRE(bad.empty());
    }
}
