#ifndef _LOGGER_
#define _LOGGER_

#pragma once
#include <string>

namespace mpc
{

    class Logger
    {

    private:
        std::string path;

    public:
        static Logger l;

    public:
        void log(std::string s) const;
        void setPath(const std::string &s);

    public:
        Logger();
    };
} // namespace mpc

#define MLOG(arg) (mpc::Logger::l.log(arg))
#endif
