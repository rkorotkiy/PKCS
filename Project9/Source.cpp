#include <iostream>
#include <Windows.h>
#include <libloaderapi.h>
#include "pkcs11.h"

static HINSTANCE LoadLib(const wchar_t* pkcs11lib) {

	HINSTANCE hLib = LoadLibrary(pkcs11lib);
	return hLib;
}

void* LoadProc(HINSTANCE hLib, const char* funcname) {

	void* ldfunc = (void*)GetProcAddress(hLib, funcname);
	return ldfunc;
}

typedef int (*C_Initialize_decl)(void*);
typedef int (*C_GetInfo_decl)(CK_INFO*);
typedef int (*C_GetFunctionList_decl)(CK_FUNCTION_LIST**);

int (*C_Init)(void*);
int (*C_GetI)(CK_INFO*);
int (*C_GetFuncList)(CK_FUNCTION_LIST**);

int main() {
	HINSTANCE H = LoadLib(L"C:\\SoftHSM2\\lib\\softhsm2.dll");

	C_GetFuncList = (C_GetFunctionList_decl)LoadProc(H, "C_GetFunctionList");
	CK_FUNCTION_LIST_PTR test_list;
	C_GetFuncList(&test_list);

	CK_C_Initialize pC_Initialize = test_list->C_Initialize;

	if (pC_Initialize(NULL_PTR) == CKR_OK) {
		std::cout << "Initialized" << std::endl;
	}
	else {
		std::cout << "Error";
		return 1;
	}

	CK_C_GetInfo pC_GetInfo = test_list->C_GetInfo;
	CK_INFO test_info;
	
	pC_GetInfo(&test_info);
	std::cout << (int)test_info.cryptokiVersion.major;

}