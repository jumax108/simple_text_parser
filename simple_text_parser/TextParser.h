#pragma once
class TextParser{

public:

	TextParser(const char* fileName);

	bool GetValueByKey(
		const char* const key, // value를 찾을 key 값
		char** out // value 출력
	);

	bool GetTextByKey(
		const char* const key, // value를 찾을 key 값
		char** out // value 출력
	);
private:

	char* data;
	int idx = 0;
	int size;

	void SkipNotValue();

	bool GetNextWord(
		char** out
	);

	bool GetNextText(
		char** out
	);



};