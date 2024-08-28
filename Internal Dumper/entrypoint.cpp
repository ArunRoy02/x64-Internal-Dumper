#include "includes.h"

void Initialize ( )
{
	// Sleep ( 60000 );

	static bool dumpAllModules = false;
	utilities::Dumper ( dumpAllModules );
}

bool __fastcall DllMain ( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	if ( fdwReason == DLL_PROCESS_ATTACH )
	{
		Initialize ( );
	}

	return true;
}