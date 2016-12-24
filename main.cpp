#define CONTEXT_HUFFMAN
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#ifndef CONTEXT_HUFFMAN
#include "huffman.h"
#endif


#ifdef CONTEXT_HUFFMAN
#include "huffmanContext.h"
#endif

#include <iostream>
#include <fstream>

using namespace std;
int main(void)
{
	//fstream file("data_static.txt");
	//fstream file("D:\\ГУАП\\Магистратура\\3й семестр\\Кодирование\\assert\\enwik8");
	FILE *f2;
	//f2 = fopen("D:\\ГУАП\\Магистратура\\3й семестр\\Кодирование\\assert\\enwik8", "rb");	
	f2 = fopen("data_static.txt", "rb");

	fseek(f2, 0L, SEEK_END);
	int size = ftell(f2);
	fseek(f2, 0L, SEEK_END);
	rewind(f2);

	//int size = 0;
	//size = 100000000;
	char *inputData = new char[size];
	fread(inputData, size, 1, f2);
	//file.seekg(0, std::ios::end);
	//size = file.tellg();
	//file.seekg(0, 0);
	//char *inputData = new char[size+1];
	//file.read(inputData, size);
	//inputData[size] = '\0';
	//We build the tree depending on the string
#ifndef CONTEXT_HUFFMAN
	htTree *codeTree = buildTree(inputData, size);
#endif

#ifdef CONTEXT_HUFFMAN
	htTreeCont *codeTreeCont = buildTreeCont(inputData, size);
#endif
	printf("\ncode tree end");


	//We build the table depending on the Huffman tree
#ifndef CONTEXT_HUFFMAN
	hlTable *codeTable = buildTable(codeTree);
#endif

#ifdef CONTEXT_HUFFMAN
	hlTableCont *codeTable = buildTableCont(codeTreeCont);
#endif
	printf("\ncreating table end");

	//We encode using the Huffman table
	int numOfBits = -1;
#ifndef CONTEXT_HUFFMAN
	encode(codeTable, inputData, size, &numOfBits);
#endif

#ifdef CONTEXT_HUFFMAN
	encodeCont(codeTable, inputData, size, &numOfBits);
#endif
	printf("\nencode end");
	FILE *f;
	f = fopen("encode_text.txt", "rb");
	fseek(f, 0L, SEEK_END);
	size = ftell(f);
	fseek(f, 0L, SEEK_END);
	rewind(f);
	//file.close();
	//We decode using the Huffman tree
	//We can decode string that only use symbols from the initial string
	//file.open("encode_text.txt");
	//file("encode_text.txt");
	//size = 0;
	//file.seekg(0, std::ios::end);
	//size = file.tellg();
	//file.seekg(0, 0);
	//size = 100000000;
	char *inputencodeData = new char[size];
	//file.read(inputencodeData, size);
	//inputencodeData[size] = '\0';
	fread(inputencodeData, size, 1, f);

#ifndef CONTEXT_HUFFMAN
	decode(codeTree, inputencodeData, size, numOfBits);
#endif

#ifdef CONTEXT_HUFFMAN
	decodeCont(codeTreeCont, inputencodeData, size, numOfBits);
#endif
	printf("\ndecode end");
	//Output : 0011 1110 1011 0001 0010 1010 1100 1111 1000 1001
	//file.close();
	//We decode using the Huffman tree
	//We can decode string that only use symbols from the initial string
	//file.open("decode_text.txt");
	//file("encode_text.txt");
	//size = 0;
	//file.seekg(0, std::ios::end);
	//size = file.tellg();
	//file.seekg(0, 0);
	//char *inputencodeData2 = new char[size+1];
	//file.read(inputencodeData2, size);
	//inputencodeData2[size] = '\0';
	printf("\nthe end");
	getchar();
	return 0;
}