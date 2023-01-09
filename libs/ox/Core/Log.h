#pragma once

#include <cstdarg>
#include <stdio.h>
#include <iostream>

namespace ox
{
	class Log
	{
	public:
		static void info(const char* format, ...)
		{
			va_list args;
			char buf[2048];
			va_start(args, format);
			vsnprintf(buf, sizeof(buf), format, args);
			va_end(args);

			std::cout << "[I] " << buf << std::endl;
		}

		static void Error(const char* format, ...)
		{
			va_list args;
			char buf[2048];
			va_start(args, format);
			vsnprintf(buf, sizeof(buf), format, args);
			va_end(args);

			std::cerr << "[E] " << buf << std::endl;
		}

	private:
		static void _log(const char* title, const char* format, va_list args)
		{
			char buf[1024];
			//va_start(args, format);
			vsnprintf(buf, sizeof(buf), format, args);
			//va_end(args);
			std::cout << title << buf << std::endl;
		}
	};
}
