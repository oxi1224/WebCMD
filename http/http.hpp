#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <time.h>
#include <functional>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <bitset>
#include "response.hpp"
#include "request.hpp"
#include "cookie.hpp"
#include "util.hpp"
#include "constants.hpp"

#pragma comment(lib, "Ws2_32.lib")

const int BUFF_SIZE = 1024;
namespace fs = std::filesystem;

namespace http {
	using namespace http;
	using namespace http::util;
	typedef std::function<void(Request*, Response*)> Callback;
	typedef std::map<std::string, std::map<std::string, Callback>> ListenerMap;

	std::map<std::string, std::string> mimeTypes{
		{".aac", "audio/aac"},
		{".abw", "application/x-abiword"},
		{".apng", "image/apng"},
		{".arc", "application/x-freearc"},
		{".avif", "image/avif"},
		{".avi", "video/x-msvideo"},
		{".azw", "application/vnd.amazon.ebook"},
		{".bin", "application/octet-stream"},
		{".bmp", "image/bmp"},
		{".bz", "application/x-bzip"},
		{".bz2", "application/x-bzip2"},
		{".cda", "application/x-cdf"},
		{".csh", "application/x-csh"},
		{".css", "text/css"},
		{".csv", "text/csv"},
		{".doc", "application/msword"},
		{".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
		{".eot", "application/vnd.ms-fontobject"},
		{".epub", "application/epub+zip"},
		{".gz", "application/gzip"},
		{".gif", "image/gif"},
		{".htm", "text/html"},
		{".html", "text/html"},
		{".ico", "image/vnd.microsoft.icon"},
		{".ics", "text/calendar"},
		{".jar", "application/java-archive"},
		{".jpeg", "image/jpeg"},
		{".js", "text/javascript"},
		{".json", "application/json"},
		{".jsonld", "application/ld+json"},
		{".mid", "audio/midi"},
		{".mjs", "text/javascript"},
		{".mp3", "audio/mpeg"},
		{".mp4", "video/mp4"},
		{".mpeg", "video/mpeg"},
		{".mpkg", "application/vnd.apple.installer+xml"},
		{".odp", "application/vnd.oasis.opendocument.presentation"},
		{".ods", "application/vnd.oasis.opendocument.spreadsheet"},
		{".odt", "application/vnd.oasis.opendocument.text"},
		{".oga", "audio/ogg"},
		{".ogv", "video/ogg"},
		{".ogx", "application/ogg"},
		{".opus", "audio/opus"},
		{".otf", "font/otf"},
		{".png", "image/png"},
		{".pdf", "application/pdf"},
		{".php", "application/x-httpd-php"},
		{".ppt", "application/vnd.ms-powerpoint"},
		{".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
		{".rar", "application/vnd.rar"},
		{".rtf", "application/rtf"},
		{".sh", "application/x-sh"},
		{".svg", "image/svg+xml"},
		{".tar", "application/x-tar"},
		{".tif", "image/tiff"},
		{".ts", "video/mp2t"},
		{".ttf", "font/ttf"},
		{".txt", "text/plain"},
		{".vsd", "application/vnd.visio"},
		{".wav", "audio/wav"},
		{".weba", "audio/webm"},
		{".webm", "video/webm"},
		{".webp", "image/webp"},
		{".woff", "font/woff"},
		{".woff2", "font/woff2"},
		{".xhtml", "application/xhtml+xml"},
		{".xls", "application/vnd.ms-excel"},
		{".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
		{".xml", "application/xml"},
		{".xul", "application/vnd.mozilla.xul+xml"},
		{".zip", "application/zip"},
		{".3gp", "video/3gpp"},
		{".3g2", "video/3gpp2"},
		{".7z", "application/x-7z-compressed"}
	};

	void AddMime(std::string extension, std::string type) {
		mimeTypes[extension] = type;
	}

	class Server {
	public:
		std::string s_address;
		int s_port;
		bool s_dev;

		Server(std::string addr, int port, bool dev = false) {
			s_address = addr;
			s_port = port;
			s_dev = dev;
		}

		~Server() {
			close();
		}

		int start() {
			if (WSAStartup(MAKEWORD(2, 0), &m_wsaData) != 0) {
				log("WSAStartup Failed: " + WSAGetLastError(), true);
				return 1;
			}

			m_sock = socket(AF_INET, SOCK_STREAM, 0);
			m_sockAddress.sin_family = AF_INET;
			m_sockAddress.sin_addr.s_addr = inet_addr(s_address.c_str());
			m_sockAddress.sin_port = htons(s_port);

			m_sockAddressLen = sizeof(m_sockAddress);

			if (bind(m_sock, (SOCKADDR*)&m_sockAddress, sizeof(m_sockAddress)) < 0) {
				log("WSAStartup Failed: " + WSAGetLastError(), true);
				return 1;
			}

			if (listen(m_sock, 10) < 0) {
				log("WSAStartup Failed: " + WSAGetLastError(), true);
				return 1;
			}

			log("Listening on address: " + s_address + ":" + std::to_string(s_port));
			loadStatic();
			listenForRequests();
		}

		void close() {
			closesocket(m_sock);
			closesocket(m_clientSock);
			WSACleanup();
			exit(0);
		}

		void assign(std::string path, std::string method, Callback callback) {
			auto pathEntry = m_listeners.find(path);
			if (pathEntry == m_listeners.end()) {
				m_listeners[path] = std::map<std::string, Callback>{ {method, callback} };
			} else {
				pathEntry->second[method] = callback;
			}
		}

		void addIntercept(std::string path, std::string method, Callback callback) {
			auto pathEntry = m_intercepts.find(path);
			if (pathEntry == m_intercepts.end()) {
				m_intercepts[path] = std::map<std::string, Callback>{ {method, callback} };
			} else {
				pathEntry->second[method] = callback;
			}
		}

		void setStaticFolderPath(std::string path) {
			m_staticPath = path;
		}
	private:
		void listenForRequests() {
			while (true) {
				m_clientSock = accept(m_sock, (SOCKADDR*)&m_sockAddress, (int*)&m_sockAddressLen);

				if (m_clientSock < 0) {
					log("Connection rejected", true);
					continue;
				}
				
				char requestBuffer[BUFF_SIZE] = {};
				int receivedBytes = recv(m_clientSock, requestBuffer, sizeof(requestBuffer), 0);
				Request req = Request(std::string(requestBuffer));
				Response res = Response();

				auto pathEntry = m_listeners.find(req.path);
				if (pathEntry != m_listeners.end()) {
					auto methodEntry = pathEntry->second.find(req.method);
					if (methodEntry != pathEntry->second.end()) {
						methodEntry->second(&req, &res);
					}
				}

				auto interceptPathEntry = m_intercepts.find(req.path);
				if (interceptPathEntry != m_intercepts.end()) {
					auto interceptMethodEntry = interceptPathEntry->second.find(req.method);
					if (interceptMethodEntry != interceptPathEntry->second.end()) {
						interceptMethodEntry->second(&req, &res);
					}
				}
				
				std::string stringRes = res.toString();
				send(m_clientSock, stringRes.c_str(), stringRes.size(), 0);
				closesocket(m_clientSock);
			}
		}

		void loadStatic() {
			if (m_staticPath.empty()) return;
			bool dirExists = fs::exists(m_staticPath);
			if (!dirExists) {
				log("Provided static directory path does not exist. (" + m_staticPath + ")", true);
				log("Consider using an absolute path or make sure the path is correct.", true);
				exit(1);
			}

			fs::recursive_directory_iterator staticIter(m_staticPath);
			for (const auto& entry : staticIter) {
				if (entry.is_directory()) {
					continue;
				}
				std::string stringPath = entry.path().generic_string();
				std::string relativePath = stringPath.substr(m_staticPath.length());
				size_t fileExtensionPos = relativePath.find_last_of(".");
				std::string fileExtension = relativePath.substr(fileExtensionPos);
				std::string assignPath = relativePath;
				size_t lastSlashPos = relativePath.find_last_of("/");
				std::string filename = relativePath.substr(lastSlashPos + 1, (fileExtensionPos - 1) - (lastSlashPos + 1) + 1);
				if ((fileExtension == ".html" || fileExtension == ".htm") && filename == "index") {
					assignPath = relativePath.substr(0, lastSlashPos + 1);
				}

				std::string lastChar(1, assignPath[assignPath.length() - 1]);
				if (lastChar == "/") {
					assign(
						assignPath.substr(0, assignPath.length() - 1),
						"GET",
						[assignPath](http::Request* req, http::Response* res) {
							res->setStatus(301);
							res->setHeader("Location", assignPath);
						}
					);
				}

				assign(
					assignPath,
					"GET",
					[&s_dev = s_dev, stringPath](http::Request* req, http::Response* res) {
						std::ifstream file(stringPath, std::ios::binary);
						std::string content;
						content.assign(
							(std::istreambuf_iterator<char>(file)),
							(std::istreambuf_iterator<char>())
						);
						size_t extensionPos = stringPath.find_last_of(".");
            std::string extension = stringPath.substr(extensionPos);
						auto mimeEntry = mimeTypes.find(extension);
						res->setStatus(500);
						if (mimeEntry != mimeTypes.end()) {
              res->setStatus(200);
							if (s_dev && (extension == ".html" || extension == ".htm")) {
								size_t bodyEndPos = content.find("</body>");
								if (bodyEndPos != std::string::npos) {
									content.insert(bodyEndPos - 1, constants::liveReloadInject);
								}
							}
							res->setContent(content, mimeEntry->second);
						} else {
							log("Mime type for file extension " + extension + " not found", true);
						}
					}
				);

				if (s_dev) {
					assign(
						"/dev/check-refresh",
						"GET",
						[this](http::Request* req, http::Response* res) {
							std::string currentUri = req->getBody();
							res->setStatus(200);
							int i = 0;
							for (const auto& entry : fs::recursive_directory_iterator(m_staticPath)) {
								std::string stringPath = entry.path().generic_string();
								unsigned int fileSize = entry.file_size();
								if (i >= m_fileSizes.size()) {
									m_fileSizes.push_back(fileSize);
								} else if (m_fileSizes[i] != fileSize) {
									res->setContent("true", "text/plain");
									m_fileSizes[i] = fileSize;
									loadStatic();
									break;
								}
								i++;
							}
;						}
					);
				}
			}
		}

		SOCKET m_sock{};
		SOCKET m_clientSock{};
		WSAData m_wsaData{};
		sockaddr_in m_sockAddress{};
		unsigned int m_sockAddressLen{};
		ListenerMap m_listeners{};
		ListenerMap m_intercepts{};
		std::string m_staticPath = "";
		std::vector<unsigned int> m_fileSizes{};
	};
}
#endif