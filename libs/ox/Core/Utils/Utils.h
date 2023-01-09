#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <filesystem>


namespace satan
{
	class Utils
	{
	public:
		static std::string now()
		{
			auto now = std::chrono::system_clock::now();
			// you need to get milliseconds explicitly
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
			// and that's a "normal" point of time with seconds
			auto t = std::chrono::system_clock::to_time_t(now);

			std::ostringstream stream;
			stream << std::put_time(localtime(&t), "%d.%m.%Y %H:%M:%S")
				<< "." << std::setfill('0') << std::setw(3) << milliseconds.count()
				<< " " << std::put_time(localtime(&t), "%z");

			return stream.str();
		}
	};
}
