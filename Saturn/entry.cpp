#include "main/main.hpp"

//saturn self leak - happy cat
//does not have clean code don't expect much :pray:

bool __stdcall DllMain(HMODULE module, ULONG reason, LPVOID reserved)
{
	if (reason == 1)
	{
		erase_pe(module);

		scan_sigs();

		hook();

		return TRUE;
	}
	return TRUE;
}