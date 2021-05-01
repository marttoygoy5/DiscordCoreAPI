// HelperFunctions.hpp - Header for my various helper functions.
// Apr 14, 2021
// Chris M.
// https://github.com/RealTimeChris

#pragma once

namespace CommanderNS {

	namespace HelperFunctions {
		const wchar_t* convertFromCharToWchar_t(const char* inputString) {
			auto lengthInCharacters = strlen(inputString) + 1;
			wchar_t* newString = new wchar_t[lengthInCharacters];
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, newString, lengthInCharacters, inputString, _TRUNCATE);
			return newString;
		}

		const char* convertFromWchar_tToChar(const wchar_t* inputString) {
			auto lengthInCharacters = wcslen(inputString);
			char* newString = new char[lengthInCharacters];
			for (unsigned int x = 0; x < lengthInCharacters; x += 1) {
				wctomb(newString, inputString[x]);
			}
			return newString;
		}
	}	
}
