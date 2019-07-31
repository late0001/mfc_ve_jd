
// mfc_ffmpeg_video_filterDlg.h : ͷ�ļ�
//
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/avutil.h>
#include "libavutil/mathematics.h"
#include "libavutil/time.h"
#include <libswscale/swscale.h>
#include <sdl/sdl.h>
#ifdef __cplusplus
}
#endif
#pragma once


// CMFC_ffmpeg_video_filterDlg �Ի���
class CMFC_ffmpeg_streamerDlg : public CDialogEx
{
// ����
public:
	SDL_Window *sdl_win;
	CMFC_ffmpeg_streamerDlg(CWnd* pParent = NULL);	// ��׼���캯��
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_FFMPEG_STREAMER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CStatusBar m_statusbar;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//int SetStatusMessage(CString str);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int SetStatusMessage(wchar_t *buf);
protected:
	afx_msg LRESULT OnUserMsg1(WPARAM wParam, LPARAM lParam);
};
