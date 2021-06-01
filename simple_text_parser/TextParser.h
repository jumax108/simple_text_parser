#pragma once
class TextParser{

public:

	TextParser(const char* fileName);

	bool GetValueByKey(
		const char* const key, // value�� ã�� key ��
		char** out // value ���
	);

	bool GetTextByKey(
		const char* const key, // value�� ã�� key ��
		char** out // value ���
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