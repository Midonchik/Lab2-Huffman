#pragma once
#ifndef _HUFFMANCONT_H
#define _HUFFMANCONT_H

//The Huffman tree node definition
typedef struct _htNodeCont {
	char symbol[2];
	struct _htNodeCont *left, *right;
}htNodeCont;

/*
	We "encapsulate" the entire tree in a structure
	because in the future we might add fields like "size"
	if we need to. This way we don't have to modify the entire
	source code.
*/
typedef struct _htTreeCont {
	htNodeCont *root;
}htTreeCont;

//The Huffman table nodes (linked list implementation)
typedef struct _hlNodeCont {
	char symbol[2];
	char *code;
	struct _hlNodeCont *next;
}hlNodeCont;

//Incapsularea listei
typedef struct _hlTableCont {
	hlNodeCont *first;
	hlNodeCont *last;
}hlTableCont;

htTreeCont * buildTreeCont(char *inputString, int size);
hlTableCont * buildTableCont(htTreeCont *huffmanTree);
void encodeCont(hlTableCont *table, char *stringToEncode, int stringSize, int *numofBits);
void decodeCont(htTreeCont *tree, char *stringToDecode, int stringSize, int numofBits);

#endif