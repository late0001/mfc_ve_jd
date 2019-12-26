//#include "stdafx.h"
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
