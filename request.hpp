#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>
#include <sstream>

namespace http {
	class Request {
	public:
		std::string path;
		std::string method;

		Request(std::string rawRequest) {
			m_rawRequest = rawRequest;
			parseRequest();
		}

		void parseRequest() {
			std::istringstream stream(m_rawRequest);
			std::string line;

			// get method, path and params
			std::getline(stream, line);
			std::istringstream pathLineStream(line);
			std::string _path;
			std::getline(pathLineStream, method, ' ');
			std::getline(pathLineStream, _path, ' ');
			std::getline(pathLineStream, m_httpVersion);

			size_t paramsPos = _path.find("?");
			if (paramsPos != std::string::npos) {
				path = _path.substr(0, paramsPos);
				parseParams(_path.substr(paramsPos + 1));
			} else {
				path = _path;
			}

			// get headers
			while (std::getline(stream, line) && !line.empty()) {
				if (!line.empty() && line.back() == '\r') {
					line.pop_back();
				}
				std::istringstream lineStream(line);
				std::string header, value;
				if (std::getline(lineStream, header, ':') && std::getline(lineStream, value)) {
					value.erase(value.find_first_of(' '), 1);
					m_headers[header] = value;
				}
			}
			m_body = stream.str();
		}

		std::string getHeader(std::string header) {
			auto entry = m_headers.find(header);
			if (entry == m_headers.end()) {
				return "";
			}
			return entry->second;
		}

		std::string getParam(std::string param) {
			auto entry = m_params.find(param);
			if (entry == m_params.end()) {
				return "";
			}
			return entry->second;
		}

		std::string getBody() {
			return m_body;
		}
	private:
		void parseParams(std::string rawParams) {
			std::istringstream lineStream(rawParams);
			std::string key, val;
			while (std::getline(lineStream, key, '=') && std::getline(lineStream, val, '&')) {
				m_params[key] = val;
			}
		}

		std::string m_rawRequest;
		std::string m_httpVersion;
		std::string m_body;
		std::map<std::string, std::string> m_params{};
		std::map<std::string, std::string> m_headers{};
	};
}
#endif