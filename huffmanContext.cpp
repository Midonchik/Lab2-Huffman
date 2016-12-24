#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bitset>
#include "huffmanContext.h"
#include "pQueueContext.h"

using namespace std;

void traverseTree(htNodeCont *treeNode, hlTableCont **table, int k, char code[256])
{
	//If we reach the end we introduce the code in the table
	if(treeNode->left == NULL && treeNode->right == NULL)
	{
		code[k] = '\0';
		hlNodeCont *aux = (hlNodeCont *)malloc(sizeof(hlNodeCont));
		aux->code = (char *)malloc(sizeof(char)*(strlen(code)+1));
		strcpy(aux->code,code);
		aux->symbol[0] = treeNode->symbol[0];
		aux->symbol[1] = treeNode->symbol[1];

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


hlTableCont * buildTableCont(htTreeCont * huffmanTree)
{
	//We initialize the table
	hlTableCont *table = (hlTableCont *)malloc(sizeof(hlTableCont));
	table->first = NULL;
	table->last = NULL;
	
	//Auxiliary variables
	const int codeLength = 256;
	char code[codeLength];
	//k will memories the level on which the traversal is
	int k=0;

	//We traverse the tree and calculate the codes
	traverseTree(huffmanTree->root,&table,k,code);
	return table;
}

htTreeCont *buildTreeCont(char *inputString, int stringLength)
{
	//The array in which we calculate the frequency of the symbols
	//Knowing that there are only 256 posibilities of combining 8 bits
	//(256 ASCII characters)
	int tableSize = 256;
	int coef = 0;
	int ** probability = new int *[tableSize];
	for (int i = 0; i < tableSize; i++)
	{
		probability[i] = new int[tableSize];
		for (int i2 = 0; i2 < tableSize; i2++)
		{
			probability[i][i2] = 0;
		}

	}
	//We consider the symbol as an array index and we calculate how many times each symbol appears
	int count = 0;
	if ((stringLength % 2) != 0)
	{
		coef = 1;
	}
	for (int i = 0; i < (stringLength - coef); i += 2)
	{
		probability[(unsigned char)inputString[i]][(unsigned char)inputString[i + 1]]++;
		count++;
	}
	//The queue which will hold the tree nodes
	pQueueCont * huffmanQueue;
	initPQueue(&huffmanQueue);

	//We create nodes for each symbol in the string
	for (int i = 0; i < tableSize; i++)
		for (int i2 = 0; i2 < tableSize; i2++)
		{	
			if (probability[i][i2] != 0)
			{
				htNodeCont *aux = (htNodeCont *)malloc(sizeof(htNodeCont));
				aux->left = NULL;
				aux->right = NULL;
				aux->symbol[0] = (char)i;
				aux->symbol[1] = (char)i2;

				addPQueue(&huffmanQueue, aux, probability[i][i2]);
			}
		}
	//We free the array because we don't need it anymore
		for (int i = 0; i < tableSize; i++)
		{
			delete[] probability[i];
		}
		delete[] probability;

	//We apply the steps described in the article to build the tree
	while(huffmanQueue->size!=1)
	{
		int priority = huffmanQueue->first->priority;
		priority+=huffmanQueue->first->next->priority;

		htNodeCont *left = getPQueue(&huffmanQueue);
		htNodeCont *right = getPQueue(&huffmanQueue);

		htNodeCont *newNode = (htNodeCont *)malloc(sizeof(htNodeCont));
		newNode->left = left;
		newNode->right = right;

		addPQueue(&huffmanQueue,newNode,priority);
	}

	//We create the tree
	htTreeCont *tree = (htTreeCont *)malloc(sizeof(htTreeCont));

	tree->root = getPQueue(&huffmanQueue);
	
	return tree;
}

void encodeCont(hlTableCont *table, char *stringToEncode, int stringSize, int *numofBits)
{
	hlNodeCont *traversal;
	//const int size = stringSize;
	//const int num = 10;
	bitset<8> encodeBits;

	//printf("\nEncoding\nInput string : %s\nEncoded string : \n", stringToEncode);
	//For each element of the string traverse the table
	//and once we find the symbol we output the code for it
	FILE *f;
	f = fopen("encode_text.txt", "wb");
	int bitCounter = 0;
	int numOfBits = 0;
	int coef = 0;
	int numOfBytes = 0;
	if (stringSize % 2 != 0)
	{
		coef = 1;
	}
	for (int i = 0; i < stringSize - coef; i += 2)
	{
		traversal = table->first;
		int counter = 0;
		if (i == 106)
		{
			int a = 0;
		}
		while ((traversal->symbol[0] != stringToEncode[i]) || (traversal->symbol[1] != stringToEncode[i+1]))
		{
			traversal = traversal->next;
			counter++;
		}

		//
		for (int i2 = 0; traversal->code[i2] != '\0'; i2++)
		{
			if ((bitCounter % 8) == 0 && i != 0)
			{
				bitCounter = 0;
				//encodeBits.
				fwrite(&encodeBits, 1, 1, f);
				numOfBytes++;
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
	//fclose(f);
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

void decodeCont(htTreeCont *tree, char *stringToDecode, int stringSize, int numofBits)
{
	FILE *f;
	f = fopen("decode_text.txt", "wb");
	int bitsCounter = 0;
	htNodeCont *traversal = tree->root;

	//printf("\nDecoding\nInput string : %s\nDecoded string : \n",stringToDecode);

	//For each "bit" of the string to decode
	//we take a step to the left for 0
	//or ont to the right for 1
	bitset<8> bufer;
	int numOfBytes = 0;
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
				//printf("%c%c", traversal->symbol[0], traversal->symbol[1]);
				fwrite(&(traversal->symbol[0]), 1, 1, f);
				numOfBytes++;
				fwrite(&(traversal->symbol[1]), 1, 1, f);
				numOfBytes++;
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