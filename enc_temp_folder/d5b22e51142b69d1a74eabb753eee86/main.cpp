#include <iostream>
#include <stdio.h>
#include <fstream>
#include "./http/http.hpp"

const std::string OUT_FILE = "out.temp";

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

int main() {
	http::Server srv = http::Server("0.0.0.0", 4003);

	srv.assign(
		"/exec",
		"POST",
		[](http::Request* req, http::Response* res) {
			std::string body = req->getBody();
			std::string out = exec(body);

			res->setStatus(200);
			res->setContent(
				out,
				"text/plain"
			);
			res->setHeader("Access-Control-Allow-Origin", "*");
		}
	);
	srv.start();
}