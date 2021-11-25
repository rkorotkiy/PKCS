#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <libloaderapi.h>
#include "pkcs11.h"
#include "tdef.h"
#include "PKCSExceptions.h"
#include <string>

class CryptoToken {
private:
	HINSTANCE hLib;
	void* LoadFunc = NULL_PTR;
	CK_C_INITIALIZE_ARGS InitArgs;
	CK_FUNCTION_LIST_PTR FuncList = NULL_PTR;
	CK_SLOT_ID_PTR SlotList = NULL_PTR;
	CK_SLOT_INFO SlotInfo;
	CK_TOKEN_INFO TokenInfo;
	CK_ULONG ListCount;
	CK_FLAGS flags = 0;
	CK_VOID_PTR pReserved;
	CK_MECHANISM_TYPE_PTR MechanismList;
	CK_MECHANISM_INFO MechanismInfo;
	void LoadProc(HINSTANCE, const char*);
public:
	CryptoToken(const wchar_t*);
	int GetFunctionList();
	int Initialize();
	int Initialize(CK_C_INITIALIZE_ARGS);
	int GetSlotList(CK_BBOOL);
	int GetSlotInfo(unsigned int);
	int GetTokenInfo(unsigned int);
	int WaitForSlotEvent();
	int GetMechanismList();
	int GetMechanismInfo(unsigned int);
	int Finalize();
	~CryptoToken();
};

CryptoToken::CryptoToken(const wchar_t* PATH_TO_DLL) {
	hLib = LoadLibrary(PATH_TO_DLL);
}

void CryptoToken::LoadProc(HINSTANCE hLib, const char* FUNC_NAME) {
	LoadFunc = (void*)GetProcAddress(hLib, FUNC_NAME);
}

int CryptoToken::GetFunctionList() {
	if (hLib == NULL) throw  LibLoadErr();
	LoadProc(hLib, "C_GetFunctionList");
	if (LoadFunc == NULL) throw FuncLoadErr();
	int (*C_GetFuncList)(CK_FUNCTION_LIST_PTR_PTR);
	C_GetFuncList = (C_GetFunctionList_decl)LoadFunc;
	CK_RV rv = C_GetFuncList(&FuncList);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::Initialize() {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_Initialize pC_Initialize = FuncList->C_Initialize;
	CK_RV rv = pC_Initialize(NULL_PTR);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::Initialize(CK_C_INITIALIZE_ARGS InitArgs) {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_Initialize pC_Initialize = FuncList->C_Initialize;
	CK_RV rv = pC_Initialize((CK_VOID_PTR)&InitArgs);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::GetSlotList(CK_BBOOL token_present) {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_GetSlotList pC_GetSlotList = FuncList->C_GetSlotList;
	CK_RV rv = pC_GetSlotList(token_present, NULL_PTR, &ListCount);
	if (rv != CKR_OK) throw RetVal(rv);
	SlotList = new CK_SLOT_ID;
	rv = pC_GetSlotList(token_present, SlotList, &ListCount);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::GetSlotInfo(unsigned int slot) {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_GetSlotInfo pC_GetSlotInfo = FuncList->C_GetSlotInfo;
	CK_RV rv = pC_GetSlotInfo(SlotList[slot - 1], &SlotInfo);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::GetTokenInfo(unsigned int slot) {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_GetTokenInfo pC_GetTokenInfo = FuncList->C_GetTokenInfo;
	if (slot == 0) throw PKCSExceptions();
	CK_RV rv = pC_GetTokenInfo(SlotList[slot - 1], &TokenInfo);
	if (rv != CKR_OK)throw RetVal(rv);
	return rv;
}

int CryptoToken::Finalize() {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_Finalize pC_Finalize = FuncList->C_Finalize;
	CK_RV rv = pC_Finalize(NULL_PTR);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::WaitForSlotEvent() {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_WaitForSlotEvent pC_WaitForSlotEvent = FuncList->C_WaitForSlotEvent;
	CK_RV rv = pC_WaitForSlotEvent(flags, SlotList, pReserved);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::GetMechanismList() {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_GetMechanismList pC_GetMechanismList = FuncList->C_GetMechanismList;
	CK_RV rv = pC_GetMechanismList(*SlotList, NULL_PTR, &ListCount);
	if (rv != CKR_OK)throw RetVal(rv);
	if (ListCount > 0) MechanismList = new CK_MECHANISM_TYPE;
	rv = pC_GetMechanismList(*SlotList, MechanismList, &ListCount);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

int CryptoToken::GetMechanismInfo(unsigned int slot) {
	if (FuncList == NULL) throw FuncListErr();
	CK_C_GetMechanismInfo pC_GetMechanismInfo = FuncList->C_GetMechanismInfo;
	CK_RV rv = pC_GetMechanismInfo(*SlotList, MechanismList[slot], &MechanismInfo);
	if (rv != CKR_OK) throw RetVal(rv);
	return rv;
}

CryptoToken::~CryptoToken() {
	if (SlotList != NULL_PTR) {
		delete SlotList;
	}
	if (MechanismList != NULL_PTR) {
		delete MechanismList;
	}
	FreeLibrary(hLib);
}

int main() {
	
	CryptoToken a(L"C:\\SoftHSM2\\lib\\softhsm2-x64.dll");
	try {
		a.GetFunctionList();
	}
	catch (LibLoadErr ex) {
		std::cout << ex.what();
		exit(1);
	}
	catch (FuncLoadErr ex) {
		std::cout << ex.what();
		exit(1);
	}
	catch (RetVal ex) {
		std::cout << ex.what();
		exit(1);
	}
	a.Initialize();
	a.GetSlotList(FALSE);
	try {
		a.GetSlotInfo(100);
	}
	catch (RetVal ex) {
		std::cout << ex.what();
		exit(1);
	}

}