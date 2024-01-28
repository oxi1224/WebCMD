#ifndef HTTP_UTIL_HPP
#define HTTP_UTIL_HPP

#include <iostream>
#include <chrono>
#include <string>

namespace http {
	namespace util {
		void log(std::string message, bool err = false) {
			auto now = std::chrono::system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(now);
			struct tm* parts = std::localtime(&now_c);
			std::string strHr = std::to_string(parts->tm_hour);
			std::string strMin = std::to_string(parts->tm_min);
			std::string strSec = std::to_string(parts->tm_sec);

			if (strHr.length() != 2) {
				strHr = "0" + strHr;
			}
			if (strMin.length() != 2) {
				strMin = "0" + strMin;
			}
			if (strSec.length() != 2) {
				strSec = "0" + strSec;
			}

			std::string timestamp = "[";
			timestamp += strHr;
			timestamp += ":";
			timestamp += strMin;
			timestamp += ":";
			timestamp += strSec;
			timestamp += "] ";

			if (err) {
				std::cerr << timestamp << message << std::endl;
			} else {
				std::cout << timestamp << message << std::endl;
			}
		}
	}
}

#endif