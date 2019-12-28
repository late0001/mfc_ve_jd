#include "stdafx.h"
#include "log.h"
char *pSzLogFileName = "mfc_video_filter.log";
void init_log()
{
	FILE *plogfile = fopen(pSzLogFileName, "wb+");
	fclose(plogfile);
}

int logd(char *buf)
{
	if (buf == NULL) return -1;

	FILE *plogfile = fopen(pSzLogFileName, "ab");
	fwrite(buf, sizeof(char), strlen(buf), plogfile);
	//fflush(plogfile);
	fclose(plogfile);
	return 0;
}

void log_print(LPCTSTR lpszFmt, ...)
{
	va_list args;
	va_start(args, lpszFmt);
	int len = _vsctprintf(lpszFmt, args) + 1;
	TCHAR *lpszBuf = (TCHAR*)_alloca(len * sizeof(TCHAR));//栈中分配, 不需要释放
	_vstprintf_s(lpszBuf, len, lpszFmt, args);
	va_end(args);
	OutputDebugString(lpszBuf);
}