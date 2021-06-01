#include <stdio.h>
#include <malloc.h>
#include "myLinkedList.h"
#include "TextParser.h"

int main() {

	TextParser textParser("test.txt");

	char* text = nullptr;

	textParser.GetValueByKey("bbb", &text);
	textParser.GetTextByKey("ccc", &text);
	printf("%s", text);

	return 0;
}