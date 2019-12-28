//#pragma once
#ifndef __LOG_H__
#define __LOG_H__
#include <string.h>
#include <stdio.h>

void init_log();
int logd(char *buf);
void log_print(LPCTSTR lpszFmt, ...);
#endif

