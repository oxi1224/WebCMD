#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <random>
#include <time.h>
#include <algorithm>
#include "./http/http.hpp"
#include "./util.hpp"

using namespace Util;

const std::string OUT_FILE = "out.temp";
std::map<std::string, std::string> ENV{};

struct VerifiedUUID {
	std::string uuid;
	long int expireTime;
};
std::vector<VerifiedUUID> uuids{};

std::map<int, std::map<std::string, std::string>> terminals{};
int terminalCount = 0;

static std::string exec(std::string command) {
	std::string fullCmd = command + " > " + OUT_FILE + " 2>&1";
	std::system(fullCmd.c_str());
	std::ifstream out(OUT_FILE);
	std::string content;
	content.assign(
		(std::istreambuf_iterator<char>(out)),
		(std::istreambuf_iterator<char>())
	);
	return content;
}

static bool isAuthorized(std::string uuid) {
	if (uuid.empty()) {
		return false;
	}
	for (int i = 0; i < uuids.size(); i++) {
		VerifiedUUID data = uuids[i];
		if (data.uuid != uuid) {
			continue;
		}
		std::time_t now = std::time(0);
		if (now > data.expireTime) {
			uuids.erase(uuids.begin() + i);
			return false;
		} else {
			return true;
		}
	}
	return false;
}

static std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

static std::string sanitize(std::string string) {
	ReplaceAll(string, "\\", "\\\\"); // backslash
	ReplaceAll(string, "\n", "\\n");	// new-line
	ReplaceAll(string, "\r", "\\r");	// carriage return
	ReplaceAll(string, "\t", "\\t");	// tab
	ReplaceAll(string, "\"", "\\\""); // quote
	return string;
}

static void printHelp() {
	std::cout << "--help " << "               Displays information about the app" << std::endl;
	std::cout << "--dev " << "                If present, uses dev mode (live reloading)" << std::endl;
	std::cout << "--staticPath [PATH] " << "  Sets the path to the served static website files" << std::endl;
	std::cout << "--addr [ADDRES] " << "      Sets the address which the server will try to use" << std::endl;
	std::cout << "--port [PORT] " << "        Sets the port which the server will try to use" << std::endl;
}

int main(int argc, char* argv[]) {
	loadEnv(&ENV);
	std::string staticPath = "./static";
	std::string addres = "0.0.0.0";
	int port = 4003;
	bool isDevEnv = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--dev") == 0) {
			isDevEnv = true;
			http::util::log("Using dev environment");
		} else if (strcmp(argv[i], "--staticPath") == 0 && (i + 1) < argc) {
			staticPath = argv[i + 1];
			i += 1;
			http::util::log("Changed default path to: " + staticPath);
		} else if (strcmp(argv[i], "--addr") == 0 && (i + 1) < argc) {
			addres = argv[i + 1];
			i += 1;
			http::util::log("Changed address to " + addres);
		} else if (strcmp(argv[i], "--port") == 0 && (i + 1) < argc) {
			port = std::stoi(argv[i + 1]);
			i += 1;
			http::util::log("Changed port to " + std::to_string(port));
		} else {
			printHelp();
			return 0;
		}
	}

	http::Server srv = http::Server(addres, port, isDevEnv);
	srv.setStaticFolderPath(staticPath);

	srv.assign(
		"/api/exec",
		"POST",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (!isAuthorized(uuid)) {
				res->setStatus(401);
				res->setContent("Unauthorized", "text/plain");
				return;
			}
			std::string body = req->getBody();
			std::istringstream stream(body);
			std::string stringTerminalId, command;
			std::getline(stream, stringTerminalId);
			std::getline(stream, command);
			int terminalId = std::stoi(stringTerminalId);
			auto entry = terminals.find(terminalId);
			if (entry == terminals.end()) {
				res->setStatus(400);
			} else {
				std::string out = exec(command);
				entry->second[command] = out;
				res->setStatus(200);
				res->setContent(
					out,
					"text/plain"
				);
			}
		}
	);

	srv.assign(
		"/api/login",
		"POST",
		[](http::Request* req, http::Response* res) {
			std::string body = req->getBody();
			std::istringstream stream(body);
			std::string login, password;
			std::getline(stream, login);
			std::getline(stream, password);
			
			std::string resContent = "";
			if (login != ENV["WEB_LOGIN"] || password != ENV["WEB_PASSWD"]) {
				res->setStatus(401);
				resContent = "Invalid credentials";
			} else {
				res->setStatus(200);
				std::string uuid = getUUID();
				http::Cookie uuidCookie;
				uuidCookie.name = "id";
				uuidCookie.value = uuid;
				uuidCookie.path = "/";
				uuidCookie.sameSite = "Strict";
				uuidCookie.httpOnly = true;
				uuidCookie.setExpiryTime(3600);
				res->addCookie(uuidCookie.toString());

				VerifiedUUID data;
				data.uuid = uuid;
				time_t now = time(0);
				long int offsetTime = now + 3600;
				data.expireTime = offsetTime;
				uuids.push_back(data);
			}
			res->setContent(resContent, "text/plain");
		}
	);

	srv.assign(
		"/api/createTerminal",
		"POST",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (!isAuthorized(uuid)) {
				res->setStatus(401);
				res->setContent("Unauthorized", "text/plain");
				return;
			}
			terminals[terminalCount] = std::map<std::string, std::string>{};
			res->setStatus(200);
			res->setContent(std::to_string(terminalCount), "text/plain");
			terminalCount += 1;
		}
	);

	srv.assign(
		"/api/closeTerminal",
		"POST",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (!isAuthorized(uuid)) {
				res->setStatus(401);
				res->setContent("Unauthorized", "text/plain");
				return;
			}
			int terminalId = std::stoi(req->getBody());
			terminals.erase(terminalId);
			res->setStatus(200);
		}
	);

	srv.assign(
		"/api/getTerminal",
		"GET",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (!isAuthorized(uuid)) {
				res->setStatus(401);
				res->setContent("Unauthorized", "text/plain");
				return;
			}
			std::string stringId = req->getParam("id");
			int id = std::stoi(stringId);
			auto entry = terminals.find(id);
			if (entry == terminals.end()) {
				res->setStatus(404);
			} else {
				std::ostringstream out;
				out << "[";
				for (auto const& cmdEntry : entry->second) {
					std::string cmdOut = sanitize(cmdEntry.second);
					out << "[\"" << cmdEntry.first << "\", ";
					out << "\"" << cmdOut << "\"" << "], ";
				}
				out << "]";
				std::string stringOut = out.str();
				size_t lastComma = stringOut.find_last_of(',');
				if (lastComma != std::string::npos) {
					stringOut.erase(lastComma, 1);
				}
				res->setStatus(200);
				res->setContent(stringOut, "application/json");
			}
		}
	);

	srv.assign(
		"/api/getTerminalIDs",
		"GET",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (!isAuthorized(uuid)) {
				res->setStatus(401);
				res->setContent("Unauthorized", "text/plain");
				return;
			}
			std::ostringstream out;
			out << "[";
			for (const auto& terminal : terminals) {
				out << terminal.first << ", ";
			}
			out << "]";
			std::string outString = out.str();
			size_t lastComma = outString.find_last_of(',');
			if (lastComma != std::string::npos) {
				outString.erase(lastComma, 1);
			}
			res->setStatus(200);
			res->setContent(outString, "application/json");
		}
	);
	
	srv.addIntercept(
		"/",
		"GET",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (!isAuthorized(uuid)) {
				res->setStatus(303);
				res->setHeader("Location", "/login/");
			}
		}
	);

	srv.addIntercept(
		"/login/",
		"GET",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (isAuthorized(uuid)) {
				res->setStatus(303);
				res->setHeader("Location", "/");
			}
		}
	);

	srv.start();
}