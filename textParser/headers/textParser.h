#pragma once

#include <stdio.h>
#include <Windows.h>

#include "dump/headers/dump.h"
#pragma comment(lib, "lib/dump/dump")

#include "objectFreeList/headers/objectFreeList.h"

#include "linkedList/headers/linkedList.h"


class CTextParser{

public:

	CTextParser(const char* fileName);

	bool setNameSpace(const char* name);
	void resetNameSpace();

	bool getIntByKey(
		const char* key, // value�� ã�� key ��
		int* out // value ���
	);

	bool getTextByKey(
		const char* key, // value�� ã�� key ��
		char* out, // value ���
		int outBufLen, // out buffer ũ��
		int* outLen // ����� text�� ũ��
	);

private:

	char* _fileBuf;
	int _index;
	int _fileSize;

	bool getNextWord(char* out, int outBufLen, int* wordLen);
	bool getNextText(char* out, int outBufLen, int* wordLen);

	struct stKeyValue {
		char _key[30];
		char _value[100];
		int _valueLen;
		stKeyValue(){
			ZeroMemory(_key, 30);
			ZeroMemory(_value, 100);
			_valueLen = 0;
		}
	};

	struct stNameSpace {
		char _name[30];
		stNameSpace* _parent;
		CLinkedList<stKeyValue*> _keyValueList;
		CLinkedList<stNameSpace*> _nameSpaceList;
		stNameSpace(){
			_parent = nullptr;
			ZeroMemory(_name, 30);
		}
	};

	// ��� namespace�� default namespace ������ �����Ѵ�.
	stNameSpace* _defaultNameSpace;

	// ������ name space
	stNameSpace* _nowNameSpace;

	CObjectFreeList<stKeyValue> _keyValueFreeList;
	CObjectFreeList<stNameSpace> _nameSpaceFreeList;

};