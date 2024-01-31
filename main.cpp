#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <random>
#include <time.h>
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

static std::string exec(std::string command) {
	std::string fullCmd = command + " > " + OUT_FILE;
	std::system(fullCmd.c_str());
	std::ifstream out(OUT_FILE);
	std::string content;
	content.assign(
		(std::istreambuf_iterator<char>(out)),
		(std::istreambuf_iterator<char>())
	);
	return content;
}

int main(int argc, char* argv[]) {
	loadEnv(&ENV);
	bool isDevEnv = false;
	if (argc > 1 && strcmp(argv[1], "--dev") == 0) {
		isDevEnv = true;
		http::util::log("Using dev environment");
	}
	http::Server srv = http::Server("0.0.0.0", 4003, isDevEnv);
	srv.setStaticFolderPath("C:/Users/oxi12/Desktop/WebCMD/static");
	srv.assign(
		"/exec",
		"POST",
		[](http::Request* req, http::Response* res) {
			std::string uuid = req->getCookie("id");
			if (uuid.empty()) {
				res->setStatus(401);
				res->setContent("Unauthorized", "text/plain");
				return;
			}
			std::string body = req->getBody();
			std::string out = exec(body);
			res->setStatus(200);
			res->setContent(
				out,
				"text/plain"
			);
		}
	);

	srv.assign(
		"/login",
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
				time_t offsetTime = now + 3600;
				data.expireTime = (long int)time(&offsetTime);
				uuids.push_back(data);
			}
			res->setContent(resContent, "text/plain");
		}
	);

	srv.start();
}