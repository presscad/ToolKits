#pragma once
#include "Buffer.h"
#include "f_ent_list.h"
class CStackMemory{
protected:
	//0~3 Bytes��ʾMAT��Memory alloc table)���ĳ���M��,����M����ΪMAT��������Ϊջ�ڴ�������
	char* memory;
public:
	char* AllocMemoryBlock(int nAllocSize) { return NULL; }
	bool  FreeMemoryBlock(void* pMemAddr, int nMemSize) { return false; }
	bool  CheckMemoryInStack(void* pMemAddr, int nMemSize) { return false; }
};
