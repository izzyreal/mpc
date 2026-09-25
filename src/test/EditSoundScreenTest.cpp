#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "TestMpc.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/TrimScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
using namespace mpc;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

static std::shared_ptr<Sound> addDiscardTestSound(Mpc &mpc, const char *name,
                                                  int frames, bool mono)
{
    const auto s = mpc.getSampler()->addSound();
    s->setName(name);
    auto data = std::make_shared<std::vector<float>>(frames * (mono ? 1 : 2));
    for (size_t i = 0; i < data->size(); ++i)
    {
        (*data)[i] = float(i + 1) / 1024.0f;
    }
    s->setSampleData(data);
    s->setMono(mono);
    return s;
}

static void checkDiscard(const int sortMode, const bool mono,
                         const bool exceedsOther)
{
    CAPTURE(sortMode, mono, exceedsOther);
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    std::shared_ptr<Sound> selected, other;
    // Choose insertion order so NAME/SIZE place the selected sound at the
    // raw index of the shorter sound, reproducing the original mismatch.
    if (sortMode == 2)
    {
        selected = addDiscardTestSound(mpc, "A-LONG", 128, mono);
        other = addDiscardTestSound(mpc, "Z-SHORT", 32, mono);
        mpc.getSampler()->setSoundIndex(0);
    }
    else
    {
        other = addDiscardTestSound(mpc, "Z-SHORT", 32, mono);
        selected = addDiscardTestSound(mpc, "A-LONG", 128, mono);
        mpc.getSampler()->setSoundIndex(1);
    }
    auto ls = mpc.getLayeredScreen();
    ls->openScreenById(ScreenId::TrimScreen);
    auto trim = mpc.screens->get<ScreenId::TrimScreen>();
    for (int i = 0; i < sortMode; ++i)
    {
        trim->function(0);
        ls->closeCurrentScreen(); // Dismiss the sorting popup.
    }
    REQUIRE(mpc.getSampler()->getSound() == selected);
    // Both ranges are valid for the selected sound. The larger one used to
    // crash when mistakenly applied to the unrelated 32-frame sound.
    const int start = exceedsOther ? 64 : 8;
    const int end = exceedsOther ? 96 : 16;
    ls->setFocus("st");
    trim->turnWheel(start);
    ls->setFocus("end");
    trim->turnWheel(end - 128);
    REQUIRE(selected->getStart() == start);
    REQUIRE(selected->getEnd() == end);
    auto originalSelected = *selected->getSampleData();
    auto originalOther = *other->getSampleData();
    std::vector<float> expected(originalSelected.begin() + start,
                                originalSelected.begin() + end);
    if (!mono)
    {
        expected.insert(expected.end(), originalSelected.begin() + 128 + start,
                        originalSelected.begin() + 128 + end);
    }
    trim->function(4);
    REQUIRE(ls->getCurrentScreenId() == ScreenId::EditSoundScreen);
    mpc.screens->get<ScreenId::EditSoundScreen>()->function(4);
    CHECK(*selected->getSampleData() == expected);
    CHECK(*other->getSampleData() == originalOther);
    CHECK(selected->getFrameCount() == end - start);
    CHECK(selected->getStart() == 0);
    CHECK(selected->getEnd() == end - start);
    CHECK(mpc.getSampler()->getSound() == selected);
}

TEST_CASE("DISCARD must edit the selected sound with NAME and SIZE sorting",
          "[edit-sound][discard]")
{
    const auto sortMode = GENERATE(0, 1, 2);
    const auto mono = GENERATE(true, false);
    checkDiscard(sortMode, mono, false);
}
TEST_CASE("DISCARD sorted range exceeds the unrelated sound",
          "[edit-sound][discard]")
{
    const auto sortMode = GENERATE(0, 1, 2);
    const auto mono = GENERATE(true, false);
    checkDiscard(sortMode, mono, true);
}
