#pragma once

#include <array>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

typedef std::vector<std::string> StringsVector;

namespace service {
	std::string replace(const std::string& source, const char* p1, const char* p2 = nullptr, const char* p3 = nullptr);
	bool replaceSubstr(std::string* source, const char* from, const char* to);

	StringsVector split(const std::string& input, const char& delimiter);
	StringsVector loadFileToVector(const std::string& filename);
}

namespace encoding {
	std::string utf16_to_cp1251(const std::wstring& utf16);
	std::wstring cp1251_to_utf16(const std::string& cp1251);

	std::wstring utf8_to_utf16(const std::string& utf8);
    std::string utf8_to_cp1251(const std::string& utf8);
}