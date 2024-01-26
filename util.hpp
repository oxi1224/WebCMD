#ifndef UTIL_HPP
#define UTIL_HPP

namespace Util {
	void loadEnv(std::map<std::string, std::string>* map) {
		std::ifstream env(".env");
		if (!env.is_open()) {
			std::cerr << "Failed to load .env file" << std::endl;
			return;
		}
		std::string line;
		while (std::getline(env, line)) {
			std::istringstream lineStream(line);
			std::string key, value;
			if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
				key.erase(0, key.find_first_not_of(" \t\r\n"));
				key.erase(key.find_last_not_of(" \t\r\n") + 1);
				value.erase(0, value.find_first_not_of(" \t\r\n"));
				value.erase(value.find_last_not_of(" \t\r\n") + 1);
				value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
				(*map)[key] = value;
			}
		}
		env.close();
	}

	std::string getUUID() {
		static std::random_device dev;
		static std::mt19937 rng(dev());
		std::uniform_int_distribution<int> dist(0, 15);

		const char* v = "0123456789abcdef";
		const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };
		std::string res;
		for (int i = 0; i < 16; i++) {
			if (dash[i]) res += "-";
			res += v[dist(rng)];
			res += v[dist(rng)];
		}
		return res;
	}
}

#endif
