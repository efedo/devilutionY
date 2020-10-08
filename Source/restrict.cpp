#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

namespace dvl {

void ReadOnlyTest()
{
	FILE *f;
	char path[MAX_PATH], Filename[MAX_PATH];

	GetPrefPath(path, MAX_PATH);
	snprintf(Filename, MAX_PATH, "%sDiablo1ReadOnlyTest.foo", path);

	f = fopen(Filename, "wt");
	if (!f) {
		DirErrorDlg(path);
	}

	fclose(f);
	remove(Filename);;
}

}
