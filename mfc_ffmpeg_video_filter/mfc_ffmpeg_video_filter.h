
// mfc_ffmpeg_video_filter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFC_ffmpeg_video_filterApp: 
// �йش����ʵ�֣������ mfc_ffmpeg_video_filter.cpp
//

class CMFC_ffmpeg_video_filterApp : public CWinApp
{
public:
	CMFC_ffmpeg_video_filterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFC_ffmpeg_video_filterApp theApp;