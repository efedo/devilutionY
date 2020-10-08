//HEADER_GOES_HERE
#ifndef __WAVE_H__
#define __WAVE_H__

namespace dvl {

void WCloseFile(HANDLE file);
int32_t WGetFileSize(HANDLE hsFile, DWORD *lpFileSizeHigh, const std::string FileName);
void WGetFileArchive(HANDLE hsFile, DWORD *retry, const std::string FileName);
bool WOpenFile(const std::string FileName, HANDLE *phsFile, bool mayNotExist);
void WReadFile(HANDLE hsFile, LPVOID buf, DWORD to_read, const std::string FileName);
int WSetFilePointer(HANDLE file1, int offset, HANDLE file2, int whence);

}

#endif /* __WAVE_H__ */
