#include "headers\textParser.h"

CTextParser::CTextParser(const char* fileName) :
	_keyValueFreeList(false, false),
	_nameSpaceFreeList(false, false)
{

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
	int readByte = (int)fread(_fileBuf, 1, _fileSize, file);
	if (readByte != _fileSize) {
		int readError = ferror(file);
		CDump::crash();
	}
	_fileBuf[_fileSize] = '\0';
	
	/////////////////////////////////////////////////////////////////
	// ���� ������ namespace, key-value�� �м��ϴ� ����
	HANDLE heap = HeapCreate(0, 0, 0);

	char* nameBuf = nullptr;
	int nameBufLen = 0;

	// ����Ʈ ���ӽ����̽� �Ҵ�
	_defaultNameSpace = _nameSpaceFreeList.allocObject();

	_nowNameSpace = _defaultNameSpace;

	// ���� ���õ� ���ӽ����̽�
	stNameSpace* nowNameSpace = _nowNameSpace;
	
 	while (_index < _fileSize) {
		
		char* word = (char*)HeapAlloc(heap, HEAP_ZERO_MEMORY, 100);
		int len = 0;

		getNextWord(word, 100, &len);
		
		///////////////////////////////////////////////////////////////////////////
		// namespace�� �����̴�.
		if(strcmp(word, "}") == 0) {
			nowNameSpace = nowNameSpace->_parent;
			HeapFree(heap, 0, word);
			continue;
		}
		///////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////
		// �� �� ã�� �ܾ�� namespace, key�� �̸��� �ȴ�.
		if (nameBuf == nullptr) {
			nameBuf = word;
			nameBufLen = len;
			continue;
		}
		///////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////
		// nameBuf�� nullptr�� �ƴϰ�,
		// ���� ���� { �̶�� �̰��� namespace�� �����̴�.
		else if (strcmp(word, "{") == 0) {

			stNameSpace* nameSpace = _nameSpaceFreeList.allocObject();
			memcpy(nameSpace->_name, nameBuf, nameBufLen);
			nameSpace->_parent = nowNameSpace;
			nowNameSpace->_nameSpaceList.push_back(nameSpace);
			nowNameSpace = nameSpace;
			HeapFree(heap, 0, nameBuf);
			nameBuf = nullptr;
		}
		///////////////////////////////////////////////////////////////////////////
		
		
		///////////////////////////////////////////////////////////////////////////
		// nameBuf�� nullptr�� �ƴϰ�,
		// ���� ���� : �̶�� �̰��� key�� �����̴�.
		else if (strcmp(word, ":") == 0) {
			// else�� ���� �ʱ� ���� �б�
		}
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		// �� �� ã�� �ܾ�� key�� ���� �ȴ�.
		else {
			
			// �����̶��
			if(word[0] == '"'){
				_index -= len;
				getNextText(word, 100, &len);
			}

			stKeyValue* keyValue = _keyValueFreeList.allocObject();
			memcpy(keyValue->_key, nameBuf, nameBufLen);
			memcpy(keyValue->_value, word, len);
			keyValue->_valueLen = len;
			nowNameSpace->_keyValueList.push_back(keyValue);
			HeapFree(heap, 0, nameBuf);
			nameBuf = nullptr;
		}
		///////////////////////////////////////////////////////////////////////////
		
		HeapFree(heap, 0, word);
	}
	/////////////////////////////////////////////////////////////////

	HeapDestroy(heap);

}

bool CTextParser::getNextWord(char* out, int outBufLen, int* wordLen) {
	
	char* startWord = nullptr;
	int startIndex = _index;

	for (; _index <= _fileSize;_index += 1) {

		char nowByte = '\0';
		if(_index < _fileSize){
			nowByte = _fileBuf[_index];
		}
		char nextByte = '\0';
		if(_index + 1 < _fileSize){
			nextByte = _fileBuf[_index + 1];
		}
		
		/////////////////////////////////////////////////////////////////
		// ���� �ּ� ó��
		if(nowByte == '/' && nextByte == '/'){
			while(nowByte != '\r' && nowByte != '\n'){
				_index += 1;
				nowByte = _fileBuf[_index];
				if(_index >= _fileSize){
					return false;
				}
			}
			continue;
		}
		/////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////
		// ������ �ּ� ó��
		if(nowByte == '/' && nextByte == '*'){
			while(nowByte != '*' || nextByte != '/'){
				_index += 1;
				nowByte = _fileBuf[_index];
				nextByte = _fileBuf[_index + 1];

				if(_index >= _fileSize){
					return false;
				}
			}
			_index += 1;
			continue;
		}
		/////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////
		// ���� �ܾ��� ������ ��ã���� ���
		if (startWord == nullptr) {

			if (nowByte == ' ' || nowByte == '\r' || nowByte == '\n') {
				continue;
			}
			startWord = &_fileBuf[_index];
			continue;
		}
		/////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////
		// �ܾ��� ������ ã�� ��
		if (nowByte == ' ' || nowByte == '\r' || nowByte == '\n' || nowByte == '\0') {
			// �ܾ��� ���Ḧ ã��

			*wordLen = (int)(&_fileBuf[_index] - startWord);
			if (outBufLen < *wordLen) {
				// �غ�� ���۰� ������ ����
				_index = startIndex;
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

bool CTextParser::getNextText(char* out, int outBufLen, int* wordLen){

	char* startText = nullptr;
	int startIndex = _index;

	for(; _index < _fileSize; ++_index){

		char nowByte = '\0';
		if(_index < _fileSize){
			nowByte = _fileBuf[_index];
		}
		char nextByte = '\0';
		if(_index + 1 < _fileSize){
			nextByte = _fileBuf[_index + 1];
		}
		
		/////////////////////////////////////////////////////////////////
		// ���� �ּ� ó��
		if(nowByte == '/' && nextByte == '/'){
			while(nowByte != '\r' && nowByte != '\n'){
				_index += 1;
				nowByte = _fileBuf[_index];
				if(_index >= _fileSize){
					return false;
				}
			}
			continue;
		}
		/////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////
		// ������ �ּ� ó��
		if(nowByte == '/' && nextByte == '*'){
			while(nowByte != '*' || nextByte != '/'){
				_index += 1;
				nowByte = _fileBuf[_index];
				nextByte = _fileBuf[_index + 1];

				if(_index >= _fileSize){
					return false;
				}
			}
			_index += 1;
			continue;
		}
		/////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////
		// ���� ������ ������ ��ã���� ���
		if (startText == nullptr) {

			if(nowByte != '"'){
				continue;
			}

			startText = &_fileBuf[_index];
			continue;
		}
		/////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////
		// ������ ������ ã�� ��
		if (nowByte == '"') {
			// ������ ���Ḧ ã��

			*wordLen = (int)(&_fileBuf[_index] - startText);
			if (outBufLen < *wordLen) {
				// �غ�� ���۰� ������ ����
				_index = startIndex;
				return false;
			}

			// " ���Ÿ� ���� + 1 �߰�
			memcpy(out, startText + 1, *wordLen - 1);
			out[*wordLen - 1] = '\0';

			// �������� " �̱⶧���� ������ �ܾ ���� �� ������� ���� �����ϱ� ����
			_index += 1;

			return true;

		}  
		/////////////////////////////////////////////////////////////////

	}

	*wordLen = 0;
	return false;

}

bool CTextParser::getIntByKey(const char* key, int* out){

	CLinkedList<stKeyValue*>* keyValueList = &_nowNameSpace->_keyValueList;

	for(CLinkedList<stKeyValue*>::CIterator iter = keyValueList->begin(); iter != keyValueList->end(); ++iter){

		stKeyValue* keyValue = *iter;
		
		if(strcmp(key, keyValue->_key) == 0){
			*out = atoi(keyValue->_value);
			return true;
		}

	}

	return false;

}

bool CTextParser::getTextByKey(const char* key, char* out, int outBufLen, int* outLen){
	
	CLinkedList<stKeyValue*>* keyValueList = &_nowNameSpace->_keyValueList;

	for(CLinkedList<stKeyValue*>::CIterator iter = keyValueList->begin(); iter != keyValueList->end(); ++iter){

		stKeyValue* keyValue = *iter;
		
		if(strcmp(key, keyValue->_key) == 0){
			*outLen = keyValue->_valueLen;
			if( outBufLen < keyValue->_valueLen ){
				return false;
			}

			memcpy(out, keyValue->_value, keyValue->_valueLen);

			return true;
		}

	}

	*outLen = 0;
	return false;
}

bool CTextParser::setNameSpace(const char* name){

	CLinkedList<stNameSpace*>* nameSpaceList = &_nowNameSpace->_nameSpaceList;
	for(CLinkedList<stNameSpace*>::CIterator iter = nameSpaceList->begin(); iter != nameSpaceList->end(); ++iter){

		stNameSpace* nameSpace = *iter;
		if(strcmp(name, nameSpace->_name) == 0){
			_nowNameSpace = nameSpace;
			return true;
		}

	}

	return false;

}

void CTextParser::resetNameSpace(){
	_nowNameSpace = _defaultNameSpace;
}