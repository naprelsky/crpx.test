#include "Service.h"

namespace service {

    StringsVector split(const std::string& input, const char& delimiter)
	{
        StringsVector result;

        std::string token;
        std::istringstream stream(input);
        while (std::getline(stream, token, delimiter))
        {
            result.push_back(token);
        }

        return result;
	}

    // «амен€ет вхождение подстрок вида %1 %2 .. %N на соотвествующее значение из параметров
    std::string replace(const std::string& source, const char* p1, const char* p2, const char* p3)
    {
        std::string result = source;

        if (p1) while (replaceSubstr(&result, "%1", p1));
        if (p2) while (replaceSubstr(&result, "%2", p2));
        if (p3) while (replaceSubstr(&result, "%3", p3));

        return result;
    }

    bool replaceSubstr(std::string* source, const char* from, const char* to)
    {
        if (!source || !from || !to) {
            return false;
        }

        size_t length = std::char_traits<char>::length(from);
        if (source->empty() || !length) {
            return false;
        }

        size_t pos = source->find(from);
        if (pos == std::string::npos) {
            return false;
        }

        source->replace(pos, length, to);
        return true;
    }

    StringsVector loadFileToVector(const std::string& filename)
    {
        StringsVector result;
        std::ifstream file(filename);

        std::string line;
        while (std::getline(file, line)) {
            StringsVector lineData = service::split(line, '_');
            result.emplace_back(line);
        }

        return result;
    }
}

namespace encoding {

    static const std::array<uint16_t, 128> CP1251_UNICODE = {
        0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
        0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
        0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
        0x0098, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
        0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
        0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
        0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
        0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
        0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
        0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
        0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
        0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
        0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
        0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
        0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
        0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
    };

    std::string utf16_to_cp1251(const std::wstring& utf16) {
        std::string cp1251;

        std::map<uint16_t, unsigned char> unicode_cp1251;
        for (uint16_t i = 0; i < CP1251_UNICODE.size(); ++i) {
            unicode_cp1251[CP1251_UNICODE[i]] = i + 0x80;
        }

        for (wchar_t currentChar : utf16) {
            if (unicode_cp1251.find(currentChar) != unicode_cp1251.end()) {
                cp1251 += unicode_cp1251[currentChar];
            }
            else if (currentChar < 0x80) {
                cp1251 += static_cast<char>(currentChar);
            }
            else {
                cp1251 += "?";
            }
        }

        return cp1251;
    }

    std::wstring utf8_to_utf16(const std::string& utf8) {
        std::wstring utf16;
        wchar_t tmp = 0;
        for (auto it = utf8.begin(); it != utf8.end(); ++it) {
            unsigned char currentChar = *it;
            if (currentChar <= 0x7F) {
                tmp = currentChar;
            }
            else if ((currentChar & 0xE0) == 0xC0) {
                tmp = (currentChar & 0x1F) << 6;
                if (it != utf8.end() - 1) {
                    tmp |= *(++it) & 0x3F;
                }
            }
            else if ((currentChar & 0xF0) == 0xE0) {
                tmp = (currentChar & 0x0F) << 12;
                if (it != utf8.end() - 1) {
                    tmp |= (*(++it) & 0x3F) << 6;
                }
                if (it != utf8.end() - 1) {
                    tmp |= (*(++it) & 0x3F);
                }
            }
            utf16 += tmp;
        }
        return utf16;
    }

    std::wstring cp1251_to_utf16(const std::string& cp1251) {
        std::wstring out;
        for (unsigned char currentChar : cp1251) {
            out += (currentChar >= 0x80) ? CP1251_UNICODE[currentChar - 0x80] : currentChar;
        }
        return out;
    }

    std::string utf8_to_cp1251(const std::string& utf8) {
        return utf16_to_cp1251(utf8_to_utf16(utf8));
    }
}