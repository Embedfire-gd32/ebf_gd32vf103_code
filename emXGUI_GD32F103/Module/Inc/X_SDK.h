#ifndef  __X_SDK_H__
#define  __X_SDK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include	"def.h"
#include	"x_libc.h"
#include	"Kernel.h"
#include	"emXGUI.h"

#ifndef WORD
#define WORD    U16
#endif

#ifndef DWORD
#define DWORD   U32
#endif

#ifndef	VOID
#define	VOID	void
#endif

typedef DWORD*      LPDWORD;
typedef const char* LPCSTR; 

/*===================================================================================*/


/*===================================================================================*/
//// CreateFile flags
#define	F_ACCESS_MASK		(0x3<<0)
#define	F_READ				(0x0<<0)	//文件可读
#define	F_WRITE				(0x1<<0)	//文件可写
#define	F_READ_WRITE		(0x2<<0)	//文件可读写

#define	F_CREATE_MASK		(0x3<<2)
#define F_OPEN_EXISTING    	(0x0<<2)	/*文件必须已经存在,由设备提出要求*/
#define F_CREATE_NEW       	(0x1<<2)	/*创建文件;如文件存在则会出错*/
#define F_CREATE_ALWAYS    	(0x2<<2)	/*创建文件，会改写前一个文件*/
#define F_OPEN_ALWAYS      	(0x3<<2)	/*如文件不存在则创建它*/


#define	INVALID_HANDLE_VALUE	((HANDLE)0xFFFFFFFF)
#define INVALID_FILE_SIZE 		(DWORD)0xFFFFFFFF

#define HFILE_ERROR ((HANDLE)-1)

#define FILE_BEGIN           SEEK_SET
#define FILE_CURRENT         SEEK_CUR
#define FILE_END             SEEK_END
/*===================================================================================*/

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

/*===================================================================================*/

typedef	int	(*LPTHREAD_START_ROUTINE)(void *Argv);

/*===================================================================================*/

HANDLE	CreateFileA(LPCSTR lpFileName,UINT Flags,UINT Attr);
HANDLE	CreateFileW(LPCWSTR lpFileName,UINT Flags,UINT Attr);

BOOL	CloseFile(HANDLE hFile);
DWORD	SetFilePointer(HANDLE hFile,LONG lDistanceToMove,LONG* lpDistanceToMoveHigh,DWORD dwMoveMethod);
BOOL	ReadFile(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD  lpNumberOfBytesRead);
BOOL	WriteFile(HANDLE hFile,LPCVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD  lpNumberOfBytesRead);
BOOL	DeleteFileA(LPCSTR lpFileName);
BOOL	DeleteFileW(LPCWSTR lpFileName);
DWORD	GetFileSize(HANDLE hFile,LPDWORD lpFileSizeHigh);
BOOL	FlushFileBuffers(HANDLE hFile);

void	Sleep(DWORD dwMilliseconds);
void	GetLocalTime(LPSYSTEMTIME lpSystemTime);
BOOL	SetLocalTime(CONST SYSTEMTIME *lpSystemTime);
DWORD	GetTickCount(VOID);
HANDLE	CreateThread(DWORD dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
HANDLE	GetCurrentThread(VOID);
DWORD	GetCurrentThreadId(VOID);
BOOL	SetThreadPriority(HANDLE hThread,int nPriority);
int		GetThreadPriority(HANDLE hThread);
VOID	ExitThread(DWORD dwExitCode);
BOOL	TerminateThread(HANDLE hThread,DWORD dwExitCode);
DWORD	SuspendThread(HANDLE hThread);
DWORD	ResumeThread(HANDLE hThread);

/*===================================================================================*/

BOOL BMP_DrawFromFile(HDC hdc,int x,int y,const char *path);
void CaptureScreenToBMPFile(const char *out_file);
#if 0
BOOL PNG_Draw(HDC hdc,int x,int y,const u8 *png_dat,int png_size);
BOOL PNG_DrawFromFile(HDC hdc,int x,int y,const char* path);
#endif
/*==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif
