#pragma once

#include <stdio.h>
#include <Windows.h>

#include "dump/headers/dump.h"
#pragma comment(lib, "lib/dump/dump")

class CTextParser{

public:

	CTextParser(const char* fileName);

	bool GetIntByKey(
		const char* const key, // value�� ã�� key ��
		int* out // value ���
	);

	bool GetTextByKey(
		const char* const key, // value�� ã�� key ��
		char* out // value ���
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