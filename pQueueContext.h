#pragma once
#ifndef _PQUEUECONT_H
#define _PQUEUECONT_H

#include "huffmanContext.h"

//We modify the data type to hold pointers to Huffman tree nodes
#define TYPE htNodeCont *

#define MAX_SZ 65536//256*256

typedef struct _pQueueNodeCont {
	TYPE val;
	unsigned int priority;
	struct _pQueueNodeCont *next;
}pQueueNodeCont;

typedef struct _pQueueCont {
	unsigned int size;
	pQueueNodeCont *first;
}pQueueCont;

void initPQueue(pQueueCont **queue);
void addPQueue(pQueueCont **queue, TYPE val, unsigned int priority);
TYPE getPQueue(pQueueCont **queue);

#endif