#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bitset>
#include "huffman.h"
#include "pQueue.h"

using namespace std;

void traverseTree(htNode *treeNode, hlTable **table, int k, char code[256])
{
	//If we reach the end we introduce the code in the table
	if(treeNode->left == NULL && treeNode->right == NULL)
	{
		code[k] = '\0';
		hlNode *aux = (hlNode *)malloc(sizeof(hlNode));
		aux->code = (char *)malloc(sizeof(char)*(strlen(code)+1));
		strcpy(aux->code,code);
		aux->symbol = treeNode->symbol;
		aux->next = NULL;
		if((*table)->first == NULL)
		{
			(*table)->first = aux;
			(*table)->last = aux;
		}
		else
		{
			(*table)->last->next = aux;
			(*table)->last = aux;
		}
		
	}
	
	//We concatenate a 0 for each step to the left
	if(treeNode->left!=NULL)
	{
		code[k]='0';
		traverseTree(treeNode->left,table,k+1,code);
		
	}
	//We concatenate a 1 for each step to the right
	if(treeNode->right!=NULL)
	{
		code[k]='1';
		traverseTree(treeNode->right,table,k+1,code);
		
	}
}

hlTable * buildTable(htTree * huffmanTree)
{
	//We initialize the table
	hlTable *table = (hlTable *)malloc(sizeof(hlTable));
	table->first = NULL;
	table->last = NULL;
	
	//Auxiliary variables
	char code[256];
	//k will memories the level on which the traversal is
	int k=0;

	//We traverse the tree and calculate the codes
	traverseTree(huffmanTree->root,&table,k,code);
	return table;
}

htTree * buildTree(char *inputString, int stringLength)
{
	//The array in which we calculate the frequency of the symbols
	//Knowing that there are only 256 posibilities of combining 8 bits
	//(256 ASCII characters)
	int * probability = (int *)malloc(sizeof(int)*256);
	
	//We initialize the array
	for(int i=0; i<256; i++)
		probability[i]=0;

	//We consider the symbol as an array index and we calculate how many times each symbol appears
	int count = 0;
	for (int i = 0; i < stringLength; i++)
	{
		probability[(unsigned char)inputString[i]]++;
		count++;
	}
	//The queue which will hold the tree nodes
	pQueue * huffmanQueue;
	initPQueue(&huffmanQueue);

	//We create nodes for each symbol in the string
	for(int i=0; i<256; i++)
		if(probability[i]!=0)
		{
			htNode *aux = (htNode *)malloc(sizeof(htNode));
			aux->left = NULL;
			aux->right = NULL;
			aux->symbol = (char) i;
			
			addPQueue(&huffmanQueue,aux,probability[i]);
		}

	//We free the array because we don't need it anymore
	free(probability);

	//We apply the steps described in the article to build the tree
	while(huffmanQueue->size!=1)
	{
		int priority = huffmanQueue->first->priority;
		priority+=huffmanQueue->first->next->priority;

		htNode *left = getPQueue(&huffmanQueue);
		htNode *right = getPQueue(&huffmanQueue);

		htNode *newNode = (htNode *)malloc(sizeof(htNode));
		newNode->left = left;
		newNode->right = right;

		addPQueue(&huffmanQueue,newNode,priority);
	}

	//We create the tree
	htTree *tree = (htTree *) malloc(sizeof(htTree));

	tree->root = getPQueue(&huffmanQueue);
	
	return tree;
}

void encode(hlTable *table, char *stringToEncode, int stringSize, int *numofBits)
{
	hlNode *traversal;
	//const int size = stringSize;
	//const int num = 10;
	bitset<256> encodeBits;

	//printf("\nEncoding\nInput string : %s\nEncoded string : \n", stringToEncode);
	//For each element of the string traverse the table
	//and once we find the symbol we output the code for it
	FILE *f;
	f = fopen("encode_text.txt", "wb");
	unsigned long bitCounter = 0;
	int numOfBits = 0;
	//int abv = 100000000;

	for (unsigned int i = 0; i < stringSize; i++)
	{
		traversal = table->first;
		while (traversal->symbol != stringToEncode[i])
		{
			traversal = traversal->next;
		}

		//
		//bitCounter = 0;
		for (unsigned int i2 = 0; traversal->code[i2] != '\0'; i2++)
		{
			if ((bitCounter % 8) == 0)
			{
				bitCounter = 0;
				//encodeBits.
				//char buf = encodeBits
				fwrite(&encodeBits, 1, 1, f);
				//fclose(f);
			}
			if (traversal->code[i2] == '0')
				encodeBits[bitCounter] = 0;
			else
				encodeBits[bitCounter] = 1;
			bitCounter++;
			numOfBits++;
		}
		//printf("%s", traversal->code);
	}
	*numofBits = numOfBits;
	if ((bitCounter%8) != 0)
	{
		for (int i = bitCounter % 8; i < 8; i++)
		{
			encodeBits[i] = 0;
		}
			fwrite(&encodeBits, 1, 1, f);
	}
	fclose(f);
	//printf("\n");
}

void decode(htTree *tree, char *stringToDecode, int stringSize, int numofBits)
{
	FILE *f;
	f = fopen("decode_text.txt", "wb");
	int bitsCounter = 0;
	htNode *traversal = tree->root;

	//printf("\nDecoding\nInput string : %s\nDecoded string : \n",stringToDecode);

	//For each "bit" of the string to decode
	//we take a step to the left for 0
	//or ont to the right for 1
	bitset<8> bufer;
	for (int i = 0; i < stringSize; i++)
	{
		if(traversal->left == NULL && traversal->right == NULL)
		{
			//printf("%c",traversal->symbol);
			traversal = tree->root;
		}
		bufer = bitset<8>(stringToDecode[i]);

		for (int i2 = 0; i2 < 8; i2++)
		{			
			if (bufer[i2] == 0)
				traversal = traversal->left;

			if (bufer[i2] == 1)
				traversal = traversal->right;

			if (bufer[i2] != 0 && bufer[i2] != 1)
			{
				printf("The input string is not coded correctly!\n");
				fclose(f);
				return;
			}
			if(traversal->left == NULL && traversal->right == NULL)
			{
				//printf("%c",traversal->symbol);
				fwrite(&(traversal->symbol), 1, 1, f);
				traversal = tree->root;
				//printf("\n");
			}
			bitsCounter++;
			if (bitsCounter == numofBits)
			{
				//end decoding, if all bits are decoded
				i = stringSize;
				break;
			}
		}
	}
	fclose(f);

}