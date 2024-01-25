#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <time.h>
#include <functional>
#include <vector>
#include "response.hpp"
#include "request.hpp"

#pragma comment(lib, "Ws2_32.lib")

const int BUFF_SIZE = 1024;

namespace http {
	using namespace http;
	
	typedef std::function<void(Request*, Response*)> Callback;
		
	struct PathListener {
		std::string path;
		std::string method;
		Callback callback;
	};

	class Server {
	public:
		std::string s_address;
		int s_port;

		Server(std::string addr, int port) {
			s_address = addr;
			s_port = port;
		}

		~Server() {
			close();
		}

		int start() {
			if (WSAStartup(MAKEWORD(2, 0), &m_wsaData) != 0) {
				std::cout << "WSAStartup Failed: " << WSAGetLastError() << std::endl;
				return 1;
			}

			m_sock = socket(AF_INET, SOCK_STREAM, 0);

			m_sockAddress.sin_family = AF_INET;
			m_sockAddress.sin_addr.s_addr = inet_addr(s_address.c_str());
			m_sockAddress.sin_port = htons(s_port);

			m_sockAddressLen = sizeof(m_sockAddress);

			if (bind(m_sock, (SOCKADDR*)&m_sockAddress, sizeof(m_sockAddress)) < 0) {
				std::cout << "Socket bind failed: " << WSAGetLastError() << std::endl;
				return 1;
			}

			if (listen(m_sock, 10) < 0) {
				std::cout << "Socket listen failed: " << WSAGetLastError() << std::endl;
				return 1;
			}

			std::cout << "Listening on adddress: " << s_address;
			std::cout << ":" << s_port << std::endl;

			listenForRequests();
		}

		void close() {
			closesocket(m_sock);
			closesocket(m_clientSock);
			WSACleanup();
			exit(0);
		}

		void assign(std::string path, std::string method, Callback callback) {
			PathListener data;
			data.path = path;
			data.method = method;
			data.callback = callback;
			m_listeners.push_back(data);
		}
	private:
		void listenForRequests() {
			while (true) {
				m_clientSock = accept(m_sock, (SOCKADDR*)&m_sockAddress, (int*)&m_sockAddressLen);

				if (m_clientSock < 0) {
					std::cout << "Connection rejected" << std::endl;
					continue;
				}
				char requestBuffer[BUFF_SIZE] = {};
				int receivedBytes = recv(m_clientSock, requestBuffer, sizeof(requestBuffer), 0);
				Request req = Request(std::string(requestBuffer));
				Response res = Response();
				for (PathListener data : m_listeners) {
					if (req.path == data.path && req.method == data.method) {
						data.callback(&req, &res);
					}
				}
				std::string stringRes = res.toString();
				send(m_clientSock, stringRes.c_str(), stringRes.size(), 0);
				closesocket(m_clientSock);
			}
		}

		SOCKET m_sock{};
		SOCKET m_clientSock{};
		WSAData m_wsaData{};
		sockaddr_in m_sockAddress{};
		unsigned int m_sockAddressLen{};
		std::vector<PathListener> m_listeners{};
	};
}
#endif