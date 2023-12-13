#include <windows.h>
#include <tchar.h>
 
typedef struct _SharedBuffer {
    ULONG Data; 
} SharedBuffer, *PSharedBuffer;

int _tmain(int argc, _TCHAR* argv[]) {
    HANDLE hMapFile;
    PSharedBuffer pBuf;
 
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(SharedBuffer),
        _T("Local\\SharedMemory")
    );

    if (hMapFile == NULL) {
        _tprintf(_T("Could not create file mapping object (%d).\n"), GetLastError());
        return 1;
    }
 
    pBuf = (PSharedBuffer)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedBuffer));
    if (pBuf == NULL) {
        _tprintf(_T("Could not map view of file (%d).\n"), GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }
 
    while (1) {
        _tprintf(_T("Data from kernel mode: %lu\n"), pBuf->Data);
        Sleep(1000); // Sleep for 1 second (simulating periodic updates)
    }
 
    UnmapViewOfFile(pBuf);
 
    CloseHandle(hMapFile);

    return 0;
}
