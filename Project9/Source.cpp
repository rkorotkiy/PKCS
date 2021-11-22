#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>
#include "pkcs11.h"

class CryptoToken {
private:
	HINSTANCE hLib;
	CK_RV rv;
	CK_C_INITIALIZE_ARGS_PTR InitArgs;
	void* LoadFunc = NULL_PTR;
	CK_FUNCTION_LIST_PTR FuncList = NULL_PTR;
	CK_SLOT_ID_PTR SlotList = NULL_PTR;
	CK_SLOT_INFO SlotInfo;
	CK_TOKEN_INFO TokenInfo;
	CK_ULONG ListCount;
	void LoadProc(HINSTANCE, const char*);
public:
	CryptoToken(const wchar_t*);
	int m_C_GetFunctionList();
	int m_C_Initialize();
	int m_C_Initialize(CK_C_INITIALIZE_ARGS);
	int m_C_GetSlotList(CK_BBOOL);
	int m_C_GetSlotInfo(unsigned int);
	int m_C_GetTokenInfo(unsigned int);
	int m_C_Finalize();
	~CryptoToken();
};

CryptoToken::CryptoToken(const wchar_t* PATH_TO_DLL) {
	hLib = LoadLibrary(PATH_TO_DLL);
}

void CryptoToken::LoadProc(HINSTANCE hLib, const char* FUNC_NAME) {
	LoadFunc = (void*)GetProcAddress(hLib, FUNC_NAME);
}

int CryptoToken::m_C_GetFunctionList() {
	LoadProc(hLib, "C_GetFunctionList");
	typedef int (*C_GetFunctionList_decl)(CK_FUNCTION_LIST_PTR_PTR);
	int (*C_GetFuncList)(CK_FUNCTION_LIST_PTR_PTR);
	C_GetFuncList = (C_GetFunctionList_decl)LoadFunc;
	return C_GetFuncList(&FuncList);
}

int CryptoToken::m_C_Initialize() {
	CK_C_Initialize pC_Initialize = FuncList->C_Initialize;
	return pC_Initialize(NULL_PTR);
}

int CryptoToken::m_C_Initialize(CK_C_INITIALIZE_ARGS InitArgs) {
	CK_C_Initialize pC_Initialize = FuncList->C_Initialize;
	return pC_Initialize((CK_VOID_PTR)&InitArgs);
}

int CryptoToken::m_C_GetSlotList(CK_BBOOL token_present) {
	CK_C_GetSlotList pC_GetSlotList = FuncList->C_GetSlotList;
	rv = pC_GetSlotList(token_present, NULL_PTR, &ListCount);
	if (rv = CKR_OK) {
		SlotList = (CK_SLOT_ID_PTR)malloc(ListCount * sizeof(CK_SLOT_ID));
		return pC_GetSlotList(token_present, SlotList, &ListCount);
	}
}

int CryptoToken::m_C_GetSlotInfo(unsigned int slot = 1) {
	CK_C_GetSlotInfo pC_GetSlotInfo = FuncList->C_GetSlotInfo;
	return pC_GetSlotInfo(SlotList[slot - 1], &SlotInfo);
}

int CryptoToken::m_C_GetTokenInfo(unsigned int slot = 1) {
	CK_C_GetTokenInfo pC_GetTokenInfo = FuncList->C_GetTokenInfo;
	return pC_GetTokenInfo(SlotList[slot - 1], &TokenInfo);
}

int CryptoToken::m_C_Finalize() {
	CK_C_Finalize pC_Finalize = FuncList->C_Finalize;
	return pC_Finalize(NULL_PTR);
}

CryptoToken::~CryptoToken() {
	if (SlotList != NULL_PTR) {
		free(SlotList);
	}
}

int main() {
	CryptoToken a(L"C:\\SoftHSM2\\lib\\softhsm2-x64.dll");
	a.m_C_GetFunctionList();
	CK_RV rv = a.m_C_Initialize();
}