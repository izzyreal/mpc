#pragma once
#include <string>

namespace mpc::engine::audio::core
{

    class Encoding
    {

    private:
        static Encoding *PCM_SIGNED_;
        static Encoding *PCM_UNSIGNED_;
        std::string name;

    public:
        Encoding(std::string name);

    public:
        static Encoding *&PCM_SIGNED();
        static Encoding *&PCM_UNSIGNED();
    };
} // namespace mpc::engine::audio::core
