#pragma once
#include <string>

namespace mpc::engine::audio::core
{

    class Encoding
    {
        static Encoding *PCM_SIGNED_;
        static Encoding *PCM_UNSIGNED_;
        std::string name;

    public:
        Encoding(const std::string &name);

        static Encoding *&PCM_SIGNED();
        static Encoding *&PCM_UNSIGNED();
    };
} // namespace mpc::engine::audio::core
