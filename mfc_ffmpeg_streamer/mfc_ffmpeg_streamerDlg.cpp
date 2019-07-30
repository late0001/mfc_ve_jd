
// mfc_ffmpeg_video_filterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <stdio.h>
#include "mfc_ffmpeg_streamer.h"
#include "mfc_ffmpeg_streamerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ENABLE_YUVFILE 0




// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFC_ffmpeg_video_filterDlg 对话框



CMFC_ffmpeg_streamerDlg::CMFC_ffmpeg_streamerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_FFMPEG_STREAMER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_ffmpeg_streamerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_ffmpeg_streamerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_PLAY, &CMFC_ffmpeg_streamerDlg::OnBnClickedBtnPlay)
END_MESSAGE_MAP()


// CMFC_ffmpeg_video_filterDlg 消息处理程序

BOOL CMFC_ffmpeg_streamerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	HWND hwnd_sdl = GetDlgItem(IDC_STATIC1)->GetSafeHwnd();
	sdl_win = SDL_CreateWindowFrom(hwnd_sdl);
	SDL_SetWindowTitle(sdl_win, "SDL native window");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFC_ffmpeg_streamerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFC_ffmpeg_streamerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFC_ffmpeg_streamerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CMFC_ffmpeg_streamerDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	SDL_DestroyWindow(sdl_win);
	SDL_Quit();
	return CDialogEx::DestroyWindow();
}
#define ENABLE_YUVFILE  0 //1
const char *filter_descr = "movie=my_logo.png[wm];[in][wm]overlay=5:5[out]";
static AVFormatContext *pFormatCtx;
static AVCodecContext *pCodecCtx;
AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
static int video_stream_index = -1;
char *pSzLogFileName = "mfc_video_filter.log";
int logd(char *buf)
{
	FILE *plogfile = fopen(pSzLogFileName, "wb+");
	fwrite(buf, sizeof(buf),1, plogfile);
	fclose(plogfile);
	return 0;
}

static int open_input_file(const char *filename)
{
	int ret;
	AVCodec *dec;
	char logbuf[255];
	if ((ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL)) < 0) {
		sprintf_s(logbuf,"Cannot open input file\n");
		logd(logbuf);
		return ret;
	}

	if ((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0) {
		sprintf_s(logbuf, "Cannot find stream information\n");
		logd(logbuf);
		return ret;
	}

	/* select the video stream */
	ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
	if (ret < 0) {
		sprintf_s(logbuf, "Cannot find a video stream in the input file\n");
		logd(logbuf);
		return ret;
	}
	video_stream_index = ret;
	pCodecCtx = pFormatCtx->streams[video_stream_index]->codec;

	/* init the video decoder */
	if ((ret = avcodec_open2(pCodecCtx, dec, NULL)) < 0) {
		sprintf_s(logbuf, "Cannot open video decoder\n");
		logd(logbuf);
		return ret;
	}

	return 0;
}

static int init_filters(const char *filters_descr)
{
	char args[512];
	char logbuf[255];
	int ret;
	AVFilter *buffersrc = avfilter_get_by_name("buffer");
	AVFilter *buffersink = avfilter_get_by_name("ffbuffersink");
	AVFilterInOut *outputs = avfilter_inout_alloc();
	AVFilterInOut *inputs = avfilter_inout_alloc();
	enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
	AVBufferSinkParams *buffersink_params;

	filter_graph = avfilter_graph_alloc();

	/* buffer video source: the decoded frames from the decoder will be inserted here. */
	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->time_base.num, pCodecCtx->time_base.den,
		pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den);

	ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
		args, NULL, filter_graph);
	if (ret < 0) {
		sprintf_s(logbuf, "Cannot create buffer source\n");
		logd(logbuf);
		return ret;
	}

	/* buffer video sink: to terminate the filter chain. */
	buffersink_params = av_buffersink_params_alloc();
	buffersink_params->pixel_fmts = pix_fmts;
	ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
		NULL, buffersink_params, filter_graph);
	av_free(buffersink_params);
	if (ret < 0) {
		sprintf_s(logbuf, "Cannot create buffer sink\n");
		logd(logbuf);
		return ret;
	}

	/* Endpoints for the filter graph. */
	outputs->name = av_strdup("in");
	outputs->filter_ctx = buffersrc_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;

	inputs->name = av_strdup("out");
	inputs->filter_ctx = buffersink_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;

	if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
		&inputs, &outputs, NULL)) < 0)
		return ret;

	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		return ret;
	return 0;
}
#define SFP_REFRESH_EVENT	(SDL_USEREVENT + 1)
#define SFP_BREAK_EVENT		(SDL_USEREVENT + 2)
int thread_exit = 0;
int thread_pause = 0;
int sfp_refresh_thread(void *data) 
{
	thread_exit = 0;
	thread_pause = 0;
	while (thread_exit == 0) {
		if (!thread_pause) {
			SDL_Event event;
			event.type = SFP_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(30);
	}
	SDL_Event event;
	event.type = SFP_BREAK_EVENT;
	SDL_PushEvent(&event);
	thread_exit = 0;
	thread_pause = 0;
	return 0;
}

UINT ThreadPlayer(LPVOID lparam) 
{
	int ret;
	AVPacket packet;
	AVFrame *pFrame;
	AVFrame *pFrame_out;
	int got_frame;
	char logbuf[255];

	av_register_all();
	avfilter_register_all();

	if ((ret = open_input_file("cuc_ieschool.flv")) < 0)
		goto end;
	if ((ret = init_filters(filter_descr)) < 0)
		goto end;

#if ENABLE_YUVFILE
	FILE *fp_yuv = fopen("test.yuv", "wb+");
#endif

	SDL_Window *psdl_win;
	//SDL 2.0 Support for multiple windows
	int screen_w, screen_h;
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	SDL_Rect rect, rect1/*, rect2*/;
	SDL_Thread *video_tid;
	SDL_Event event;
	CMFC_ffmpeg_streamerDlg *filterDlg = (CMFC_ffmpeg_streamerDlg *)lparam;
	//psdl_win = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	screen_w, screen_h, SDL_WINDOW_OPENGL);
	//SDL_SetWindowSize(psdl_win, screen_w, screen_h);
	psdl_win = filterDlg->sdl_win;
	SDL_GetWindowSize(psdl_win, &screen_w, &screen_h);
	SDL_Renderer * pRender = SDL_CreateRenderer(psdl_win, -1, 0);// 0, SDL_RENDERER_ACCELERATED);

	//创建纹理
	SDL_Texture * pTexture = SDL_CreateTexture(pRender, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
	pFrame = av_frame_alloc();
	pFrame_out = av_frame_alloc();
	/* read all packets */
	while (1) {
		SDL_WaitEvent(&event);
		if (event.type == SFP_REFRESH_EVENT) {
			ret = av_read_frame(pFormatCtx, &packet);
			if (ret < 0) {
				thread_exit = 1; //sfp_refresh_thread线程会触发 SFP_BREAK_EVENT事件;
				continue;//break;
			}

			if (packet.stream_index == video_stream_index) {
				got_frame = 0;
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_frame, &packet);
				if (ret < 0) {
					sprintf_s(logbuf, "Error decoding video\n");
					logd(logbuf);
					break;
				}

				if (got_frame) {
					pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);

					/* push the decoded frame into the filtergraph */
					if (av_buffersrc_add_frame(buffersrc_ctx, pFrame) < 0) {
						sprintf_s(logbuf, "Error while feeding the filtergraph\n");
						logd(logbuf);
						break;
					}

					/* pull filtered pictures from the filtergraph */
					while (1) {
						ret = av_buffersink_get_frame(buffersink_ctx, pFrame_out);
						if (ret < 0) {
							break;
						}


						sprintf_s(logbuf, "Process 1 frame!\n");
						logd(logbuf);

						if (pFrame_out->format == AV_PIX_FMT_YUV420P) {
#if ENABLE_YUVFILE
							//Y, U, V
							for (int i = 0; i < pFrame_out->height; i++) {
								fwrite(pFrame_out->data[0] + pFrame_out->linesize[0] * i, 1, pFrame_out->width, fp_yuv);
							}
							for (int i = 0; i < pFrame_out->height / 2; i++) {
								fwrite(pFrame_out->data[1] + pFrame_out->linesize[1] * i, 1, pFrame_out->width / 2, fp_yuv);
							}
							for (int i = 0; i < pFrame_out->height / 2; i++) {
								fwrite(pFrame_out->data[2] + pFrame_out->linesize[2] * i, 1, pFrame_out->width / 2, fp_yuv);
							}
#endif
							//SaveYuvFrame(pFrame_out, pCodecCtx->width, pCodecCtx->height);

							rect.x = 0;
							rect.y = 0;
							rect.w = screen_w / 2;//pFrame_out->width;
							rect.h = screen_h / 2;// pFrame_out->height;
							rect1.x = screen_w / 2;
							rect1.y = screen_h / 2;
							rect1.w = screen_w / 2;//pFrame_out->width;
							rect1.h = screen_h / 2;// pFrame_out->height;
							//rect2.x = 0;
							//rect2.y = screen_h / 2;
							//rect2.w = screen_w / 2;
							//rect2.h = screen_h / 2;
							SDL_UpdateYUVTexture(pTexture, NULL, pFrame_out->data[0], pFrame_out->linesize[0],
								pFrame_out->data[1], pFrame_out->linesize[1],
								pFrame_out->data[2], pFrame_out->linesize[2]);
							SDL_RenderClear(pRender);
							SDL_RenderCopy(pRender, pTexture, NULL, &rect);
							SDL_RenderCopy(pRender, pTexture, NULL, &rect1);
							//SDL_RenderCopy(pRender, pTexture, NULL, &rect2);
							SDL_RenderPresent(pRender);

							//SDL_Delay(40);
						}
						av_frame_unref(pFrame_out);
					}
				}
				av_frame_unref(pFrame);
			}
			av_free_packet(&packet);
		}
		else if (event.type == SDL_QUIT) {
			thread_exit = 1;
		}
		else if (event.type == SFP_BREAK_EVENT) {
			break;
		}
	}
#if ENABLE_YUVFILE
	fclose(fp_yuv);
#endif
end:
	avfilter_graph_free(&filter_graph);
	if (pCodecCtx)
		avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);


	if (ret < 0 && ret != AVERROR_EOF) {
		char buf[1024];
		av_strerror(ret, buf, sizeof(buf));
		sprintf(logbuf, "Error occurred: %s\n", buf);
		logd(logbuf);
		return -1;
	}
	return 0;
}

void CMFC_ffmpeg_streamerDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码

	AfxBeginThread(ThreadPlayer, this);
}
