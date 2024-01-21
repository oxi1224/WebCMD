#include <iostream>
#include "./http.hpp"

int main() {
	http::Server srv = http::Server("0.0.0.0", 4003);

	srv.start();
}