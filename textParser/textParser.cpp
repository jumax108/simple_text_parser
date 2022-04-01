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
	// 파일 내용을 namespace, key-value로 분석하는 과정
	HANDLE heap = HeapCreate(0, 0, 0);

	char* nameBuf = nullptr;
	int nameBufLen = 0;

	// 디폴트 네임스페이스 할당
	_defaultNameSpace = _nameSpaceFreeList.allocObject();

	_nowNameSpace = _defaultNameSpace;

	// 현재 선택된 네임스페이스
	stNameSpace* nowNameSpace = _nowNameSpace;
	
 	while (_index < _fileSize) {
		
		char* word = (char*)HeapAlloc(heap, HEAP_ZERO_MEMORY, 100);
		int len = 0;

		getNextWord(word, 100, &len);
		
		///////////////////////////////////////////////////////////////////////////
		// namespace의 종료이다.
		if(strcmp(word, "}") == 0) {
			nowNameSpace = nowNameSpace->_parent;
			HeapFree(heap, 0, word);
			continue;
		}
		///////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////
		// 이 때 찾은 단어는 namespace, key의 이름이 된다.
		if (nameBuf == nullptr) {
			nameBuf = word;
			nameBufLen = len;
			continue;
		}
		///////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////
		// nameBuf가 nullptr이 아니고,
		// 다음 값이 { 이라면 이것은 namespace의 시작이다.
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
		// nameBuf가 nullptr이 아니고,
		// 다음 값이 : 이라면 이것은 key의 시작이다.
		else if (strcmp(word, ":") == 0) {
			// else로 가지 않기 위한 분기
		}
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		// 이 때 찾은 단어는 key의 값이 된다.
		else {
			
			// 문장이라면
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
		// 한줄 주석 처리
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
		// 여러줄 주석 처리
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
		// 아직 단어의 시작을 못찾았을 경우
		if (startWord == nullptr) {

			if (nowByte == ' ' || nowByte == '\r' || nowByte == '\n') {
				continue;
			}
			startWord = &_fileBuf[_index];
			continue;
		}
		/////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////
		// 단어의 시작을 찾고난 후
		if (nowByte == ' ' || nowByte == '\r' || nowByte == '\n' || nowByte == '\0') {
			// 단어의 종료를 찾음

			*wordLen = (int)(&_fileBuf[_index] - startWord);
			if (outBufLen < *wordLen) {
				// 준비된 버퍼가 작으면 리턴
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
		// 한줄 주석 처리
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
		// 여러줄 주석 처리
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
		// 아직 문장의 시작을 못찾았을 경우
		if (startText == nullptr) {

			if(nowByte != '"'){
				continue;
			}

			startText = &_fileBuf[_index];
			continue;
		}
		/////////////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////////////
		// 문장의 시작을 찾고난 후
		if (nowByte == '"') {
			// 문장의 종료를 찾음

			*wordLen = (int)(&_fileBuf[_index] - startText);
			if (outBufLen < *wordLen) {
				// 준비된 버퍼가 작으면 리턴
				_index = startIndex;
				return false;
			}

			// " 제거를 위한 + 1 추가
			memcpy(out, startText + 1, *wordLen - 1);
			out[*wordLen - 1] = '\0';

			// 마지막이 " 이기때문에 다음에 단어를 얻을 때 얻어지는 것을 방지하기 위함
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