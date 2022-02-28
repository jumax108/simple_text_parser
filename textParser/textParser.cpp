#include "headers\textParser.h"

CTextParser::CTextParser(const char* fileName) {

	FILE* file;
	fopen_s(&file, fileName, "rb");
	if (file == nullptr) {
		CDump::crash();
	}

	fseek(file, 0, SEEK_END);
	_fileSize = ftell(file);
	rewind(file);

	_index = 0;
	_fileBuf = (char*)malloc(_fileSize + 1);
	int readByte = fread(_fileBuf, 1, _fileSize, file);
	if (readByte != _fileSize) {
		int readError = ferror(file);
		CDump::crash();
	}
	_fileBuf[_fileSize] = '\0';

	HANDLE heap = HeapCreate(0, 0, 0);

	char* nameBuf = nullptr;
	bool isKey = false;
	char* parsingDataBuf = nullptr;
	while (_index < _fileSize) {

		char* out = (char*)HeapAlloc(heap, HEAP_ZERO_MEMORY, 100);
		int len = 0;

		GetNextWord(out, 100, &len);

		///////////////////////////////////////////////////////////////////////////
		// ���� �ƹ��͵� ã�� ���� ����
		// �� �� ã�� �ܾ�� namespace, key�� �̸��� �ȴ�.
		if (nameBuf == nullptr) {
			nameBuf = out;
		}
		///////////////////////////////////////////////////////////////////////////

		else if (strcmp(out, "{") == 0) {

		}

		///////////////////////////////////////////////////////////////////////////
		// �� �� ã�� �ܾ�� key�� ���� �ȴ�.
		else if (isKey == true) {
			
		}
		///////////////////////////////////////////////////////////////////////////

	}


}

bool CTextParser::GetNextWord(char* out, int outBufLen, int* wordLen) {
	
	char* startWord = nullptr;

	for (; _index < _fileSize;_index += 1) {

		/////////////////////////////////////////////////////////////////
		// ���� �ܾ��� ������ ��ã���� ���
		if (startWord == nullptr) {

			if (_fileBuf[_index] == ' ') {
				continue;
			}
			else if (_fileBuf[_index] == '\n') {
				continue;
			}

			startWord = &_fileBuf[_index];
			continue;
		}
		/////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////
		// �ܾ��� ������ ã�� ��
		if (_fileBuf[_index] == ' ' || _fileBuf[_index] == '\n') {
			// �ܾ��� ���Ḧ ã��
			*wordLen = &_fileBuf[_index] - startWord;
			if (outBufLen < *wordLen) {
				// �غ�� ���۰� ������ ����
				return false;
			}

			memcpy(out, startWord, *wordLen);
			return true;

		} 
		/////////////////////////////////////////////////////////////////


	}

	*wordLen = 0;
	return false;
}