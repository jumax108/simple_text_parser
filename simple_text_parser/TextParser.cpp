#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "TextParser.h"

TextParser::TextParser(const char* fileName) {

	FILE* dataFile;
	fopen_s(&dataFile, fileName, "rb");

	fseek(dataFile, 0, SEEK_END);
	size = ftell(dataFile);
	rewind(dataFile);

	data = new char[size + 1];
	fread(data, size, 1, dataFile);
	data[size] = '\0';

	fclose(dataFile);

}

// °ø¹é ÁÙ¹Ù²Þ ÅÇ Ã¼Å©
bool isBlank(const char data) {
	if (data == ' ' || data == 0x0d || data == 0x0a || data == 0x09 || data == '\0'){
		return true;
	}
	return false;
}

void TextParser::SkipNotValue() {

	while (true) {
		if ( isBlank(data[idx]) == true ) {
			idx += 1;
			continue;
		}

		if (data[idx] == '/' && data[idx + 1] == '/') {
			while (data[idx++] != 0x0d);
			continue;
		}

		if (data[idx] == '/' && data[idx + 1] == '*') {
			while ((data[idx] == '*' && data[idx+1] == '/') == false) {
				idx += 1;
			}
			idx = idx + 2;
			continue;
		}

		return;
	}

}

bool TextParser::GetNextWord(char** out) {
	
	free(*out);

	SkipNotValue();

	int start = idx;

	while (true) {
		if ( isBlank(data[idx]) == false ) {
			idx += 1;
			continue;
		}

		break;
	}

	int end = idx;
	int len = end - start;

	if (end > size) {
		return false;
	}

	*out = (char*)malloc(len + 1);

	for (int cnt = start; cnt < end; ++cnt) {
		(*out)[cnt - start] = data[cnt];
	}

	(*out)[len] = '\0';

	return true;
}

bool TextParser::GetNextText(char** out) {

	free(*out);

	SkipNotValue();

	idx += 1;
	int start = idx;
	 
	while (true) {
		if (data[idx] != '"') {
			idx += 1;
			continue;
		}

		break;
	}

	int end = idx;
	int len = end - start;

	if (end > size) {
		return false;
	}

	*out = (char*)malloc(len + 1);

	for (int cnt = start; cnt < end; ++cnt) {
		(*out)[cnt - start] = data[cnt];
	}

	(*out)[len] = '\0';

	return true;
}

bool TextParser::GetValueByKey(const char* const key, char** out) {
	
	idx = 0;
	char* strTemp = nullptr;
	while (GetNextWord(&strTemp)) {
		if (strcmp(key, strTemp) == 0) {
			bool result = GetNextWord(&strTemp);
			if (result == false) {
				return false;
			}

			if (strcmp(":", strTemp) == 0) {
				GetNextWord(&strTemp);
				if (result == false) {
					return false;
				}

				free(*out);
				int len = strlen(strTemp);
				*out = (char*)malloc(len + 1);
				strcpy_s(*out, len + 1, strTemp);
				(*out)[len] = '\0';
				return true;

			}
		}
	}

	return false;
}


bool TextParser::GetTextByKey(const char* const key, char** out) {

	idx = 0;
	char* strTemp = nullptr;
	while (GetNextWord(&strTemp)) {
		if (strcmp(key, strTemp) == 0) {
			bool result = GetNextWord(&strTemp);
			if (result == false) {
				return false;
			}

			if (strcmp(":", strTemp) == 0) {

				GetNextText(&strTemp);
				
				if (result == false) {
					return false;
				}

				free(*out);
				int len = strlen(strTemp);
				*out = (char*)malloc(len + 1);
				strcpy_s(*out, len + 1, strTemp);
				(*out)[len] = '\0';
				return true;

			}
		}
	}

	return false;

}