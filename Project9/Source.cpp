#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <libloaderapi.h>
#include "pkcs11.h"
#include "tdef.h"

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
	int Finalize();
	~CryptoToken();
};

CryptoToken::CryptoToken(const wchar_t* PATH_TO_DLL) {
	try {
		hLib = LoadLibrary(PATH_TO_DLL);
		if (hLib == NULL) throw  1;
	}
	catch (int a) {
		std::cout << "Error loading library" << std::endl;
		exit(1);
	}
}

void CryptoToken::LoadProc(HINSTANCE hLib, const char* FUNC_NAME) {
	try {
		LoadFunc = (void*)GetProcAddress(hLib, FUNC_NAME);
		if (LoadFunc == NULL) throw 1;
	}
	catch (int a) {
		std::cout << "Error loading function" << std::endl;
		exit(1);
	}
}

int CryptoToken::GetFunctionList() {
	LoadProc(hLib, "C_GetFunctionList");
	int (*C_GetFuncList)(CK_FUNCTION_LIST_PTR_PTR);
	C_GetFuncList = (C_GetFunctionList_decl)LoadFunc;
	try {
		CK_RV rv = C_GetFuncList(&FuncList);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV){
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::Initialize() {
	CK_C_Initialize pC_Initialize = FuncList->C_Initialize;
	try {
		CK_RV rv = pC_Initialize(NULL_PTR);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::Initialize(CK_C_INITIALIZE_ARGS InitArgs) {
	CK_C_Initialize pC_Initialize = FuncList->C_Initialize;
	try {
		CK_RV rv = pC_Initialize((CK_VOID_PTR)&InitArgs);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::GetSlotList(CK_BBOOL token_present) {
	CK_C_GetSlotList pC_GetSlotList = FuncList->C_GetSlotList;
	CK_RV rv = pC_GetSlotList(token_present, NULL_PTR, &ListCount);
	try {
		if (rv != CKR_OK) throw rv;
		//return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
	try {
		if (rv != CKR_OK) throw rv;
		SlotList = new CK_SLOT_ID;
		rv = pC_GetSlotList(token_present, SlotList, &ListCount);
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::GetSlotInfo(unsigned int slot) {
	CK_C_GetSlotInfo pC_GetSlotInfo = FuncList->C_GetSlotInfo;
	try {
		CK_RV rv = pC_GetSlotInfo(SlotList[slot - 1], &SlotInfo);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::GetTokenInfo(unsigned int slot) {
	CK_C_GetTokenInfo pC_GetTokenInfo = FuncList->C_GetTokenInfo;
	try {
		CK_RV rv = pC_GetTokenInfo(SlotList[slot - 1], &TokenInfo);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::Finalize() {
	CK_C_Finalize pC_Finalize = FuncList->C_Finalize;
	try {
		CK_RV rv = pC_Finalize(NULL_PTR);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::WaitForSlotEvent() {
	CK_C_WaitForSlotEvent pC_WaitForSlotEvent = FuncList->C_WaitForSlotEvent;
	try {
		CK_RV rv = pC_WaitForSlotEvent(flags, SlotList, pReserved);
		if (rv != CKR_OK) throw rv;
		return rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

int CryptoToken::GetMechanismList() {
	CK_C_GetMechanismList pC_GetMechanismList = FuncList->C_GetMechanismList;
	try {
		CK_RV rv = pC_GetMechanismList(*SlotList, NULL_PTR, &ListCount);
		if (rv != CKR_OK) throw rv;
	}
	catch (int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
	try {
		if (ListCount > 0) MechanismList = new CK_MECHANISM_TYPE;
		CK_RV rv = pC_GetMechanismList(*SlotList, MechanismList, &ListCount);
		if (rv != CKR_OK) throw rv;
	}
	catch(int RV) {
		std::cout << "Return value is not CKR_OK (Code " << RV << " )";
	}
}

/*Доделать C_GetMechanismInfo*/

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
	CryptoToken a(L"C:\\SoftHSM2\\lib\\softhsm2-x64.dl");
	a.GetFunctionList();
	if (a.Initialize() == CKR_OK) std::cout << "1";
}