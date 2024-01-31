#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <sstream>

namespace http {

	const std::map<int, std::string> StatusCodes{
		{ 100, "Continue" },
		{ 101, "Switching Protocols" },
		{ 102, "Processing" },
		{ 103, "Early Hints" },
		{ 200, "OK" },
		{ 201, "Created" },
		{ 202, "Accepted" },
		{ 203, "Non-Authoritative Information" },
		{ 204, "No Content" },
		{ 205, "Reset Content" },
		{ 206, "Partial Content" },
		{ 207, "Multi-Status" },
		{ 208, "Already Reported" },
		{ 226, "IM Used" },
		{ 300, "Multiple Choices" },
		{ 301, "Moved Permanently" },
		{ 302, "Found" },
		{ 303, "See Other" },
		{ 304, "Not Modified" },
		{ 305, "Use Proxy" },
		{ 306, "unused" },
		{ 307, "Temporary Redirect" },
		{ 308, "Permanent Redirect" },
		{ 400, "Bad Request" },
		{ 401, "Unauthorized" },
		{ 402, "Payment Required" },
		{ 403, "Forbidden" },
		{ 404, "Not Found" },
		{ 405, "Method Not Allowed" },
		{ 406, "Not Acceptable" },
		{ 407, "Proxy Authentication Required" },
		{ 408, "Request Timeout" },
		{ 409, "Conflict" },
		{ 410, "Gone" },
		{ 411, "Length Required" },
		{ 412, "Precondition Failed" },
		{ 413, "Payload Too Large" },
		{ 414, "URI Too Long" },
		{ 415, "Unsupported Media Type" },
		{ 416, "Range Not Satisfiable" },
		{ 417, "Expectation Failed" },
		{ 418, "I'm a teapot" },
		{ 421, "Misdirected Request" },
		{ 422, "Unprocessable Content" },
		{ 423, "Locked" },
		{ 424, "Failed Dependency" },
		{ 425, "Too Early" },
		{ 426, "Upgrade Required" },
		{ 428, "Precondition Required" },
		{ 429, "Too Many Requests" },
		{ 431, "Request Header Fields Too Large" },
		{ 451, "Unavailable For Legal Reasons" },
		{ 500, "Internal Server Error" },
		{ 501, "Not Implemented" },
		{ 502, "Bad Gateway" },
		{ 503, "Service Unavailable" },
		{ 504, "Gateway Timeout" },
		{ 505, "HTTP Version Not Supported" },
		{ 506, "Variant Also Negotiates" },
		{ 507, "Insufficient Storage" },
		{ 508, "Loop Detected" },
		{ 510, "Not Extended" },
		{ 511, "Network Authentication Required" }
	};

	class Response {
	public:
		void setStatus(int code) {
			auto kvp = StatusCodes.find(code);
			if (kvp == StatusCodes.end()) {
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