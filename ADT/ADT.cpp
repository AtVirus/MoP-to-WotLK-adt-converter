// ADT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <ShObjIdl.h>
#include "ADT_Mop.h"


int _tmain(int argc, _TCHAR* argv[])
{
	/*CoInitialize(nullptr);

	IFileDialog* ofd = nullptr;
	CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&ofd));
	COMDLG_FILTERSPEC filters[] = {
		{ L"ADT file", L"*.adt" }
	};


	ofd->SetDefaultExtension(L".adt");
	ofd->SetFileTypes(1, filters);

	ofd->Show(nullptr);

	LPWSTR fileName = nullptr;
	IShellItem* itemSelect = nullptr;
	ofd->GetResult(&itemSelect);
	itemSelect->GetDisplayName(SIGDN_FILESYSPATH, &fileName); */
	//ADT_Mop adt(fileName);
	ADT_Mop adt(argv[1]);
	adt.convert();

	//std::cin.get();
	return 0;
}

