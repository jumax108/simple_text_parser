#include "../headers/textParser.h"
#pragma comment(lib, "../release/textParser")

int main() {

	CTextParser parser("test.txt");

	int outInt = 0;
	char outChar[100] = {0,};

	parser.getIntByKey("b", &outInt);
	printf("b : %d\n", outInt);

	int charLen;
	parser.getTextByKey("a", outChar, 100, &charLen);
	printf("a : %s(%d)\n", outChar, charLen);

	parser.setNameSpace("A");

	
	parser.getIntByKey("c", &outInt);
	printf("c : %d\n", outInt);
	
	parser.getTextByKey("d", outChar, 100, &charLen);
	printf("d : %s(%d)\n", outChar, charLen);

	return 0;
}