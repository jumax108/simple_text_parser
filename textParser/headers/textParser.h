#pragma once

#include <stdio.h>
#include <Windows.h>

#include "dump/headers/dump.h"
#pragma comment(lib, "lib/dump/dump")

class CTextParser{

public:

	CTextParser(const char* fileName);

	bool GetIntByKey(
		const char* const key, // value를 찾을 key 값
		int* out // value 출력
	);

	bool GetTextByKey(
		const char* const key, // value를 찾을 key 값
		char* out // value 출력
	);
private:

	char* _fileBuf;
	int _index;
	int _fileSize;

	void SkipNotValue();

	bool GetNextWord(char* out, int outBufLen, int* wordLen);
	bool GetNextText(char* out);

	struct stKeyValue {
		char* _key[20];
		char* _value[100];
	};

	struct stKeyValueNode {
		stKeyValue _keyValue;
		stKeyValue* _next;
	};

	struct stNameSpace {
		char* _nameSpaceName;
		stKeyValueNode* _keyValueList;
	};

	struct stNameSpaceNode {
		stNameSpace _nameSpace;
		stNameSpace* _next;
	};
	
	stNameSpaceNode* _nameSpaceList;

};