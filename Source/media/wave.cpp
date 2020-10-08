#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

namespace dvl {

void WCloseFile(HANDLE file)
{
	SFileCloseFile(file);
}

int32_t WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh, const std::string FileName)
{
	int32_t ret;

	if ((ret = SFileGetFileSize(hsFile, lpFileSizeHigh)) == 0)
		FileErrDlg(FileName);

	return ret;
}

bool WOpenFile(const std::string FileName, HANDLE *phsFile, bool mayNotExist)
{
	if (!SFileOpenFile(FileName.c_str(), phsFile))
		FileErrDlg(FileName);

	return true;
}

void WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read, const std::string FileName)
{
	if (SFileSetFilePointer(hsFile, 0, NULL, DVL_FILE_CURRENT) == -1)
		FileErrDlg(FileName);

	if (!SFileReadFile(hsFile, buf, to_read, NULL, NULL))
		FileErrDlg(FileName);
}

}
