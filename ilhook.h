#pragma once

#include <windows.h>

#define MAX_PATCH_LENGTH 0x20


enum PatchType
{
    PT_WIN32API,
    PT_CALL,
    PT_ANY,
};

enum StubOptions
{
	//directly return after newFunc called. if not set, jmp to source function.
	STUB_DIRECTLYRETURN=1,

	//set eax after newFunc called.
	STUB_OVERRIDEEAX=(1<<1),
};

struct CodePattern
{
public:
    BYTE* pattern;
    BYTE* mask;
    int length;
};

struct HookSrcObject
{
    void* addr;
    PatchType type;
    CodePattern pattern;

	struct Instruction
	{
		BYTE offset;
		BYTE length;
		WORD jmpType;
		void* destAddr;
	};
	Instruction insts[5];
	int instCount;

	//for CodePattern
	BYTE _pat[MAX_PATCH_LENGTH];
};

struct HookStubObject
{
	void* addr;
	int length;
	StubOptions options;
	int retnVal;
};

struct Registers
{
	DWORD edi;
	DWORD esi;
	DWORD ebp;
	DWORD esp;
	DWORD ebx;
	DWORD edx;
	DWORD ecx;
	DWORD eax;
};

bool InitializeHookSrcObject(HookSrcObject* obj,void* addr,bool forceAny=false);
bool InitializePattern(CodePattern* pattern,BYTE* code,BYTE* mask,DWORD len);
bool InitializeStubObject(HookStubObject* obj,void* addr,int length,int retvVal=0,DWORD options=0);

bool CalcOriAddress(HookSrcObject* obj, void** addr);
bool IsPatternMatch(void* buff,CodePattern* pat);

bool PatchHookSrc(HookSrcObject* srcObj,void* destAddr);
bool GenerateMovedCode(HookSrcObject* srcObj,BYTE* destAddr,int* length);
bool GenerateStub(HookSrcObject* srcObj,HookStubObject* stubObj,void* newFunc,char* funcArgs);

bool Hook32(HookSrcObject* srcObj,CodePattern* pre,HookStubObject* stubObj,void* newFunc,char* funcArgs);

//in asmhelper.cpp

bool GetOpInfo(BYTE* addr,int* opLength,void** relativeDestAddr);

/*
*  seq:
*  a -- eax
*  b -- ebx
*  c -- ecx
*  d -- edx
*  w -- esp
*  x -- ebp
*  y -- esi
*  z -- edi
*  1-9 and A-M -- parameter 1 to 22
*  \x01-\x16 -- parameter ref 1 to 22
*  f -- orignal function pointer
*  r -- pointer to Registers struct
*/
bool GeneratePushInsts(char* seq,BYTE* addr,int* length,DWORD** oriFuncAddr);
