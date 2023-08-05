#ifndef _LOGGER_
#define _LOGGER_

#pragma once
#include <stdio.h>
#include <string>

namespace moduru {

	class Logger {

	private:
		std::string path;

	public:
		static Logger l;

	public:
		void log(std::string s);
		void setPath(std::string s);

	public:
		Logger();

	};
}

#define MLOG(arg) (moduru::Logger::l.log(arg))
#endif