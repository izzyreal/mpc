#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace mpc
{
    class Mpc;
}
namespace mpc::sampler
{
    class Program;
}

namespace mpc::disk
{
    // One operation at a time. All public methods except isActive() are called
    // on the UI/owner thread. Destruction joins I/O without requiring UI ticks.
    class SaveOperation
    {
    public:
        using Clock = std::chrono::steady_clock;
        using Now = std::function<Clock::time_point()>;
        explicit SaveOperation(
            Mpc &, Now =
                       []
                   {
                       return Clock::now();
                   });
        ~SaveOperation();
        bool startProgram(const std::shared_ptr<sampler::Program> &,
                          const std::string &, bool replace = false,
                          std::function<void()> onExists = {});
        bool startAps(const std::string &, bool replace = false,
                      std::function<void()> onExists = {});
        void tick();
        bool isActive() const;
        void shutdown();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
} // namespace mpc::disk
