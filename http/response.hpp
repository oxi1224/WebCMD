#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <sstream>
#include "constants.hpp"

namespace http {
	class Response {
	public:
		void setStatus(int code) {
			auto kvp = constants::StatusCodes.find(code);
			if (kvp == constants::StatusCodes.end()) {
				std::cerr << "Invalid response status code provided" << std::endl;
				std::runtime_error err("Invalid response status code provided");
				throw err;
			}
			m_statusCode = kvp->first;
			m_statusText = kvp->second;
		}

		void setHeader(std::string header, std::string value) {
			m_headers[header] = value;
		}

		void setContent(std::string body, std::string contentType) {
			m_body = body;
			m_headers["Content-type"] = contentType;
		}

		void addCookie(std::string cookie) {
			m_cookies.push_back(cookie);
		}

		std::string toString() {
			std::string date = getTimeStamp();
			std::ostringstream res;
			res << "HTTP/1.1 " << m_statusCode << " " << m_statusText << std::endl;
			res << "Date: " << date << std::endl;
			for (const auto &kvp : m_headers) {
				res << kvp.first << ": " << kvp.second << std::endl;
			}
			for (std::string cookie : m_cookies) {
				res << "Set-Cookie: " << cookie << std::endl;
			}
			res << std::endl;
			res << m_body;
			return res.str();
		}

	private:
		std::string getTimeStamp() {
			char dateStr[512];
			time_t now = time(0);
			struct tm tm = *gmtime(&now);
			strftime(dateStr, sizeof(dateStr), "%a, %d %b %Y %H:%M:%S GMT", &tm);
			return dateStr;
		}

		int m_statusCode = 404;
		std::string m_statusText = "Not Found";
		std::map<std::string, std::string> m_headers{};
		std::string m_body = "";
		std::vector<std::string> m_cookies{};
	};
}

#endif