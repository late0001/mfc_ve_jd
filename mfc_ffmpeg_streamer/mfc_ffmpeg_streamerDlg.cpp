
// mfc_ffmpeg_video_filterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <stdio.h>
#include "mfc_ffmpeg_streamer.h"
#include "mfc_ffmpeg_streamerDlg.h"
#include "afxdialogex.h"
#include "ConcatDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define ENABLE_YUVFILE 0


#define WM_USER_MSG1 (WM_USER + 1)

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
	//  SetStatusMessage = 0;
	m_vset1 = 0;
}

void CMFC_ffmpeg_streamerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_PTS, m_ptsSlider);
	DDX_Control(pDX, IDC_PLAYLIST, m_playListCtrl);
}

BEGIN_MESSAGE_MAP(CMFC_ffmpeg_streamerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_PLAY, &CMFC_ffmpeg_streamerDlg::OnBnClickedBtnPlay)
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_MSG1, &CMFC_ffmpeg_streamerDlg::OnUserMsg1)
	ON_COMMAND(IDM_OPEN, &CMFC_ffmpeg_streamerDlg::OnOpenFile)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_PTS, &CMFC_ffmpeg_streamerDlg::OnNMCustomdrawSliderPts)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_PTS, &CMFC_ffmpeg_streamerDlg::OnReleasedcaptureSliderPts)
	ON_BN_CLICKED(IDC_MOVIE_CLIP, &CMFC_ffmpeg_streamerDlg::OnClickedMovieClip)
	ON_BN_CLICKED(IDC_BTN_CUTSTART, &CMFC_ffmpeg_streamerDlg::OnBnClickedBtnCutstart)
	ON_BN_CLICKED(IDC_BTN_CUTEND, &CMFC_ffmpeg_streamerDlg::OnBnClickedBtnCutend)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CMFC_ffmpeg_streamerDlg::OnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CON, &CMFC_ffmpeg_streamerDlg::OnBnClickedBtnCon)
END_MESSAGE_MAP()

static UINT indicators[] = {
	IDS_INDICATOR_MSG,
	IDS_INDICATOR_TIME
};

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
	m_menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_menu);
	//Setting status bars
	SetTimer(1, 1000, NULL);
	m_statusbar.Create(this);
	m_statusbar.SetIndicators(indicators, 2);
	CRect rect;
	GetClientRect(&rect);
	//size of two panels
	m_statusbar.SetPaneInfo(0, IDS_INDICATOR_TIME, SBPS_NORMAL, rect.Width() - 200);
	m_statusbar.SetPaneInfo(1, IDS_INDICATOR_MSG, SBPS_STRETCH, 0);
	//This is where we actually draw it on the screen
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,
		IDS_INDICATOR_TIME);
	// list control
	m_playListCtrl.InsertColumn(0, "File", LVCFMT_LEFT, 200);//插入列
	m_playListCtrl.InsertColumn(1, "Time", LVCFMT_LEFT, 50);
	//m_playListCtrl.InsertItem(0, "hello", 0);
	// SDL initialization 
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		CString str;
		str.Format("Could not initialize SDL - %s\n", SDL_GetError());
		AfxMessageBox(str);
	}
	HWND hwnd_sdl = GetDlgItem(IDC_STATIC1)->GetSafeHwnd();
	sdl_win = SDL_CreateWindowFrom(hwnd_sdl);
	SDL_SetWindowTitle(sdl_win, "SDL native window");

	//ffmpeg initilization
	av_register_all();
	avformat_network_init();
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




#define ENABLE_YUVFILE  0 //1
const char *filter_descr = "movie=my_logo.png[wm];[in][wm]overlay=5:5[out]";
static AVFormatContext *pFormatCtx;
static AVCodecContext *pCodecCtx;
AVFilterContext *buffersink_ctx;
AVFilterContext *buffersrc_ctx;
AVFilterGraph *filter_graph;
static int video_stream_index = -1;


BOOL CMFC_ffmpeg_streamerDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	CloseInput();
	SDL_DestroyWindow(sdl_win);
	SDL_Quit();
	return CDialogEx::DestroyWindow();
}

int push_streamer(CMFC_ffmpeg_streamerDlg *pdlg) 
{
	AVOutputFormat *ofmt = NULL;
	//输入对应一个AVFormatContext，输出对应一个AVFormatContext
	//（Input AVFormatContext and Output AVFormatContext）
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVPacket pkt;
	const char *in_filename, *out_filename;
	int ret, i;
	int videoindex = -1;
	int frame_index = 0;
	int64_t start_time = 0;
	char temp_buf[255];

	//in_filename  = "cuc_ieschool.mov";
	//in_filename  = "cuc_ieschool.mkv";
	//in_filename  = "cuc_ieschool.ts";
	//in_filename  = "cuc_ieschool.mp4";
	//in_filename  = "cuc_ieschool.h264";
	in_filename = "cuc_ieschool.flv";//输入URL（Input file URL）
    //in_filename  = "shanghai03_p.h264";

	out_filename = "rtmp://192.168.0.104:1935/cctvf/zm";//输出 URL（Output URL）[RTMP]
	//out_filename = "rtp://233.233.233.233:6666";//输出 URL（Output URL）[UDP]
	init_log();
	av_register_all();
	//Network
	avformat_network_init();
	//输入（Input）
	if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		AfxMessageBox("Could not open input file.");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		AfxMessageBox("Failed to retrieve input stream information");
		goto end;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++)
		if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}

	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	//输出（Output）

	avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", out_filename); //RTMP
	//avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP

	if (!ofmt_ctx) {
		AfxMessageBox("Could not create output context");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		//根据输入流创建输出流（Create output AVStream according to input AVStream）
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, NULL);
		if (!out_stream) {
			AfxMessageBox("Failed allocating output stream");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		
		//复制AVCodecContext的设置（Copy the settings of AVCodecContext）
		//ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);	
		if (ret < 0) {
			AfxMessageBox("Failed to copy context from input to output stream codec context");
			goto end;
		}
//		out_stream->codecpar->codec_tag = 0;
//		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
//			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
	//Dump Format------------------
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//打开输出URL（Open output URL）
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			//MultiByteToWideChar(CP_ACP, 0, out_filename, strlen(out_filename) + 1, temp_buf_w, 255);
			sprintf(temp_buf, "Could not open output URL '%s'", out_filename);
			//WideCharToMultiByte(CP_ACP, 0, temp_buf, wcslen(temp_buf) + 1, ansi_buf, 255, 0, 0);
			AfxMessageBox(temp_buf);
			goto end;
		}
	}
	//写文件头（Write file header）
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		AfxMessageBox("Error occurred when opening output URL\n");
		goto end;
	}

	start_time = av_gettime();
	while (1) {
		AVStream *in_stream, *out_stream;
		//获取一个AVPacket（Get an AVPacket）
		ret = av_read_frame(ifmt_ctx, &pkt);
		if (ret < 0)
			break;
		//FIX：No PTS (Example: Raw H.264)
		//Simple Write PTS
		if (pkt.pts == AV_NOPTS_VALUE) {
			//Write PTS
			AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
			//Duration between 2 frames (us)
			int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
			//Parameters
			pkt.pts = (double)(frame_index*calc_duration) / (double)(av_q2d(time_base1)*AV_TIME_BASE);
			pkt.dts = pkt.pts;
			pkt.duration = (double)calc_duration / (double)(av_q2d(time_base1)*AV_TIME_BASE);
		}
		//Important:Delay
		if (pkt.stream_index == videoindex) {
			AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
			AVRational time_base_q = { 1, AV_TIME_BASE };
			int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
			int64_t now_time = av_gettime() - start_time;
			if (pts_time > now_time)
				av_usleep(pts_time - now_time);

		}

		in_stream = ifmt_ctx->streams[pkt.stream_index];
		out_stream = ofmt_ctx->streams[pkt.stream_index];
		/* copy packet */
		//转换PTS/DTS（Convert PTS/DTS）
		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
		pkt.pos = -1;
		//Print to Screen
		if (pkt.stream_index == videoindex) {
			sprintf(temp_buf, "Send %8d video frames to output URL\n", frame_index);
			//memset(ansi_buf, 0, 255);
			//WideCharToMultiByte(CP_ACP, 0, temp_buf, wcslen(temp_buf)+1, ansi_buf, 255, 0, 0);
			logd(temp_buf);
			pdlg->SetStatusMessage(temp_buf);
			frame_index++;
		}
		//ret = av_write_frame(ofmt_ctx, &pkt);
		ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

		if (ret < 0) {
			AfxMessageBox("Error muxing packet\n");
			break;
		}

		av_packet_unref(&pkt);

	}
	//写文件尾（Write file trailer）
	av_write_trailer(ofmt_ctx);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		AfxMessageBox("Error occurred.\n");
		return -1;
	}
	AfxMessageBox("push stream end!");

	return 0;
}
int exit_thraad = 0;
int getcapdev_push_streamer(CMFC_ffmpeg_streamerDlg *pdlg)
{
	AVOutputFormat *ofmt = NULL;
	//输入对应一个AVFormatContext，输出对应一个AVFormatContext
	//（Input AVFormatContext and Output AVFormatContext）
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
	AVCodec *pdeCodex = NULL, *penCodec = NULL;
	//AVCodecContext *piCodecCtx = NULL;
	AVCodecContext *pDeCodecCtx = NULL;//
	AVCodecContext *pCodecCtx = NULL;//编码的CodecContext
	AVCodecContext *pTmpCodecCtx = NULL;
	AVPacket *dec_pkt, enc_pkt;
	AVFrame *pFrame, *pFrameYUV;
	int dec_got_frame, enc_got_frame;
	const char *in_filename, *out_filename;
	int ret, i;
	int videoindex = -1;
	int frame_index = 0;
	int64_t start_time = 0;
	char temp_buf[255];
	char device_name[50];
	struct SwsContext *img_convert_ctx;


	pDeCodecCtx = avcodec_alloc_context3(NULL);
	pTmpCodecCtx = avcodec_alloc_context3(NULL);

	in_filename = "cuc_ieschool.flv";//输入URL（Input file URL）

	out_filename = "rtmp://192.168.0.104:1935/cctvf/zm";//输出 URL（Output URL）[RTMP]
	//out_filename = "rtp://233.233.233.233:6666";//输出 URL（Output URL）[UDP]
	init_log();
	av_register_all();
	avdevice_register_all();
	//Network
	avformat_network_init();

	ifmt_ctx = avformat_alloc_context();
	AVDeviceInfoList *device_info = NULL;
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);
	AVInputFormat *iformat = av_find_input_format("dshow");
	logd("Device Info=============\n");
	//avformat_open_input(&ifmt_ctx, "video=dummy", iformat, &options);
	logd("========================\n");

	sprintf(device_name, "video=%s", "LHT-820VM31B");
	//输入（Input）
	if (avformat_open_input(&ifmt_ctx, device_name, iformat, 0) < 0) {
		AfxMessageBox("Could not open input stream.");
		goto end;
	}
	if (avformat_find_stream_info(ifmt_ctx, 0) < 0) {
		AfxMessageBox("Failed to retrieve input stream information");
		goto end;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++)
		if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		AfxMessageBox("Couldn't find a video stream.(没有找到视频流)");
		goto end;
	}
	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	avcodec_parameters_to_context(pDeCodecCtx, ifmt_ctx->streams[videoindex]->codecpar);
	//输出（Output）
	pdeCodex = avcodec_find_decoder(ifmt_ctx->streams[videoindex]->codecpar->codec_id);
	if(pdeCodex == NULL){
		AfxMessageBox("Couldn't find decoder.");
		goto end;
	}
	if (avcodec_open2(pDeCodecCtx, pdeCodex, NULL) < 0) {
		AfxMessageBox("Could not open codec.");
		goto end;
	}

	avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", out_filename); //RTMP
	//avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP
	if (!ofmt_ctx) {
		AfxMessageBox("Could not create output context");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	// output encoder initialize
	penCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!penCodec) {
		AfxMessageBox("could not find encoder!");
		goto end;
	}
	pCodecCtx = avcodec_alloc_context3(penCodec);
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = ifmt_ctx->streams[videoindex]->codecpar->width;
	pCodecCtx->height = ifmt_ctx->streams[videoindex]->codecpar->height;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	pCodecCtx->bit_rate = 400000;
	pCodecCtx->gop_size = 250;
	/* Some formats, for example flv, want stream headers to be separate. */
	if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
	//H264 codec param
	//pCodecCtx->me_range = 16;
	//pCodecCtx->max_qdiff = 4;
	//pCodecCtx->qcompress = 0.6;
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;
	//Optional Param
	pCodecCtx->max_b_frames = 3;
	//Set H264 preset and tune
	AVDictionary *param = 0;
	av_dict_set(&param, "preset", "fast", 0);
	av_dict_set(&param, "tune", "zerolatency", 0);
	ret = avcodec_open2(pCodecCtx, penCodec, &param);
	if ( ret < 0) {
		AfxMessageBox("Failed to open encoder!");
		goto end;
	}
	AVStream *video_st = avformat_new_stream(ofmt_ctx, penCodec);
	if (!video_st) {
		AfxMessageBox("Failed allocating video stream");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	
	video_st->time_base.num = 1;
	video_st->time_base.den = 25;
	//video_st->codec = pCodecCtx; //FixMe

	//Dump Format------------------
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	ofmt = ofmt_ctx->oformat;
	//打开输出URL（Open output URL）
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		//Open output URL,set before avformat_write_header() for muxing  
		if (avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0) {
			//MultiByteToWideChar(CP_ACP, 0, out_filename, strlen(out_filename) + 1, temp_buf_w, 255);
			sprintf(temp_buf, "Could not open output URL '%s'", out_filename);
			//WideCharToMultiByte(CP_ACP, 0, temp_buf, wcslen(temp_buf) + 1, ansi_buf, 255, 0, 0);
			AfxMessageBox(temp_buf);
			goto end;
		}
	}
	//写文件头（Write file header）
	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		AfxMessageBox("Error occurred when opening output URL\n");
		goto end;
	}
	//prepare before decode and encode  
	dec_pkt = (AVPacket *)av_malloc(sizeof(AVPacket));
	//enc_pkt = (AVPacket *)av_malloc(sizeof(AVPacket));  
	//camera data has a pix fmt of RGB,convert it to YUV420  
//	img_convert_ctx = sws_getContext(ifmt_ctx->streams[videoindex]->codecpar->width, ifmt_ctx->streams[videoindex]->codecpar->height,
//		ifmt_ctx->streams[videoindex]->codec->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	img_convert_ctx = sws_getContext(ifmt_ctx->streams[videoindex]->codecpar->width, ifmt_ctx->streams[videoindex]->codecpar->height,
		pDeCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	pFrameYUV = av_frame_alloc();
	//uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	//avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	uint8_t *out_buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height,1));
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
	start_time = av_gettime();
	//获取一个AVPacket（Get an AVPacket）
	while (av_read_frame(ifmt_ctx, dec_pkt) >= 0) {
		if (exit_thraad != 0) break;
		av_log(NULL, AV_LOG_DEBUG, "Going to reencode the frame\n");
		pFrame = av_frame_alloc();
		if (!pFrame) {
			ret = AVERROR(ENOMEM);
			goto end;
		}
		//av_packet_rescale_ts(dec_pkt, ifmt_ctx->streams[dec_pkt->stream_index]->time_base,  
		//  ifmt_ctx->streams[dec_pkt->stream_index]->codec->time_base);  
// 		ret = avcodec_decode_video2(ifmt_ctx->streams[dec_pkt->stream_index]->codec, pFrame,
// 			&dec_got_frame, dec_pkt);
// 		if (ret < 0) {
// 			av_frame_free(&pFrame);
// 			av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
// 			break;
// 		}
		ret = avcodec_parameters_to_context(pTmpCodecCtx, ifmt_ctx->streams[dec_pkt->stream_index]->codecpar);
		if (ret < 0) {
			sprintf(temp_buf, "avcodec_parameters_to_context failed, error code: %d\n", ret);
			OutputDebugString(temp_buf);
		}
		ret = avcodec_send_packet(pTmpCodecCtx, dec_pkt);
		dec_got_frame = avcodec_receive_frame(pTmpCodecCtx, pFrame);
		if (!dec_got_frame) {
			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

			enc_pkt.data = NULL;
			enc_pkt.size = 0;
			av_init_packet(&enc_pkt);
			//ret = avcodec_encode_video2(pCodecCtx, &enc_pkt, pFrameYUV, &enc_got_frame);
			ret = avcodec_send_frame(pCodecCtx, pFrameYUV);
			if (ret < 0) {
				OutputDebugString("Error encoding video frame: %s\n");
			}
			enc_got_frame = avcodec_receive_packet(pCodecCtx, &enc_pkt);
			av_frame_free(&pFrame);
			//if (enc_got_frame == 1) {
			if(!enc_got_frame){
				//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, enc_pkt.size);  
				sprintf(temp_buf, "Send %8d video frames to output URL\n", frame_index);
				//memset(ansi_buf, 0, 255);
				//WideCharToMultiByte(CP_ACP, 0, temp_buf, wcslen(temp_buf)+1, ansi_buf, 255, 0, 0);
				logd(temp_buf);
				pdlg->SetStatusMessage(temp_buf);
				frame_index++;
				enc_pkt.stream_index = video_st->index;

				//Write PTS  
				AVRational time_base = ofmt_ctx->streams[videoindex]->time_base;//{ 1, 1000 };  
				AVRational r_framerate1 = ifmt_ctx->streams[videoindex]->r_frame_rate;// { 50, 2 };  
				AVRational time_base_q = { 1, AV_TIME_BASE };
				//Duration between 2 frames (us)  
				int64_t calc_duration = (double)(AV_TIME_BASE)*(1 / av_q2d(r_framerate1));  //内部时间戳  
				//Parameters  
				//enc_pkt.pts = (double)(framecnt*calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));  
				enc_pkt.pts = av_rescale_q(frame_index*calc_duration, time_base_q, time_base);
				enc_pkt.dts = enc_pkt.pts;
				enc_pkt.duration = av_rescale_q(calc_duration, time_base_q, time_base); //(double)(calc_duration)*(double)(av_q2d(time_base_q)) / (double)(av_q2d(time_base));  
				enc_pkt.pos = -1;

				//Delay  
				int64_t pts_time = av_rescale_q(enc_pkt.dts, time_base, time_base_q);
				int64_t now_time = av_gettime() - start_time;
				if (pts_time > now_time)
					av_usleep(pts_time - now_time);

				ret = av_interleaved_write_frame(ofmt_ctx, &enc_pkt);
				if (ret < 0) {
					AfxMessageBox("Error muxing packet\n");
					break;
				}
				av_packet_unref(&enc_pkt);
			}
		} else {
			av_frame_free(&pFrame);
		}
		av_packet_unref(dec_pkt);

	}
	//写文件尾（Write file trailer）
	av_write_trailer(ofmt_ctx);
	sws_freeContext(img_convert_ctx);
	av_frame_free(&pFrameYUV);
end:
	avformat_close_input(&ifmt_ctx);
	/* close output */
	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		AfxMessageBox("Error occurred.\n");
		return -1;
	}
	AfxMessageBox("push stream end!");

	return 0;
}

static int open_input_file1(const char *filename)
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
	pCodecCtx = avcodec_alloc_context3(NULL);
	ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[video_stream_index]->codecpar);

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

int mffmpeg_player(CMFC_ffmpeg_streamerDlg *filterDlg)
{
	int ret;
	AVPacket packet;
	AVFrame *pFrame;
	AVFrame *pFrame_out;
	int got_frame;
	char logbuf[255];
	SDL_Texture * pTexture;

	//av_register_all();
	avfilter_register_all();
	if ((ret = open_input_file1("cuc_ieschool.flv")) < 0)
		goto end;
	if ((ret = init_filters(filter_descr)) < 0)
		goto end;

#if ENABLE_YUVFILE
	FILE *fp_yuv = fopen("test.yuv", "wb+");
#endif

	SDL_Window *psdl_win ;
	//SDL 2.0 Support for multiple windows
	int screen_w, screen_h;
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	SDL_Rect rect, rect1/*, rect2*/;
	SDL_Thread *video_tid;
	SDL_Event event;
	//psdl_win = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	screen_w, screen_h, SDL_WINDOW_OPENGL);
	//SDL_SetWindowSize(psdl_win, screen_w, screen_h);
	psdl_win = filterDlg->sdl_win;
	sprintf(logbuf, " win: 0x%p", psdl_win);
	filterDlg->SetStatusMessage(logbuf);
	SDL_GetWindowSize(psdl_win, &screen_w, &screen_h);
	SDL_Renderer * pRender = SDL_CreateRenderer(psdl_win, -1, 0);// 0, SDL_RENDERER_ACCELERATED);

	//创建纹理
	pTexture = SDL_CreateTexture(pRender, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
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
				break;//continue;//
			}

			if (packet.stream_index == video_stream_index) {
				got_frame = 0;
// 				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_frame, &packet);
// 				if (ret < 0) {
// 					sprintf_s(logbuf, "Error decoding video\n");
// 					logd(logbuf);
// 					break;
// 				}
				ret = avcodec_send_packet(pCodecCtx, &packet);
				got_frame = avcodec_receive_frame(pCodecCtx, pFrame);
				if (!got_frame) {
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
			av_packet_unref(&packet);
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
	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRender);
	//让控制线程退出
	thread_exit = 1;
	int status = 0;
	if (video_tid > 0)
		SDL_WaitThread(video_tid, &status);
	//吃掉线程退出产生的事件
	SDL_WaitEvent(&event);
	filterDlg->SetStatusMessage("Refresh Thread exit");
	if (ret < 0 && ret != AVERROR_EOF) {
		char buf[1024];
		av_strerror(ret, buf, sizeof(buf));
		sprintf(logbuf, "Error occurred: %s\n", buf);
		logd(logbuf);
		return -1;
	}
end:
	avfilter_graph_free(&filter_graph);
	if (pCodecCtx)
		avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);
	filterDlg->SetStatusMessage("close input file");
	
	
	return 0;
}

struct cvinfo {
	char *infile_name;
	AVFormatContext	*pifmt_ctx;
	int video_secs; //视频时长
	int videoindex; //视频流索引
	int64_t seek_pos;
} in_cvinfo;

struct movclip_info {
	int64_t start_time;
	int64_t end_time;
	char sv_filepath[MAX_PATH];
} movclip;

typedef struct OutputStream {
	AVStream *st;
	AVRational mux_timebase;
} OutputStream;

typedef struct InputStream {
	AVStream *st;
	AVCodecContext *dec_ctx;
	AVCodec *dec;
	int64_t filter_in_rescale_delta_last;
}InputStream;
typedef struct InputFile {
	AVFormatContext *ctx;
	AVRational time_base; /* time base of the duration */
	int       nb_streams;
	int ist_index;        /* index of first stream in input_streams */

} InputFile;

typedef struct OutputFile {
	AVFormatContext *ctx;
	int64_t recording_time; //desired length of the resulting file in microseconds == AV_TIME_BASE units
	int64_t start_time; //start time in microseconds == AV_TIME_BASE units
} OutputFile;

InputStream **input_streams = NULL;
int        nb_input_streams = 0;

void *grow_array(void *array, int elem_size, int *size, int new_size)
{
	if (new_size >= INT_MAX / elem_size) {
		OutputDebugString("Array too big.\n");
		return NULL;
	}
	if (*size < new_size) {
		uint8_t *tmp = (uint8_t *)av_realloc_array(array, new_size, elem_size);
		if (!tmp) {
			OutputDebugString("Could not alloc buffer.\n");
			return NULL;
		}
		memset(tmp + *size*elem_size, 0, (new_size - *size) * elem_size);
		*size = new_size;
		return tmp;
	}
	return array;
}

#define GROW_ARRAY(array, nb_elems)\
    array = (InputStream **)grow_array(array, sizeof(*array), &nb_elems, nb_elems + 1)

void add_input_streams(AVFormatContext *ic) 
{
	int i, ret;
	char temp_buf[255] = {0};
	in_cvinfo.videoindex = -1;
	for (i = 0; i < ic->nb_streams; i++) {
		AVStream *st = ic->streams[i];
		AVCodecParameters *par = st->codecpar;
		InputStream *ist = (InputStream *)av_mallocz(sizeof(*ist));
		if (!ist) {
			OutputDebugString("can not alloc InputStream *ist, insufficient memory!\n");
			return;
		}
		GROW_ARRAY(input_streams, nb_input_streams);
		input_streams[nb_input_streams - 1] = ist;
		ist->st = st;
		ist->dec = avcodec_find_decoder(st->codecpar->codec_id);
		ist->filter_in_rescale_delta_last = AV_NOPTS_VALUE;
		ist->dec_ctx = avcodec_alloc_context3(ist->dec);
		if (!ist->dec_ctx) {
			OutputDebugString("Error allocating the decoder context.\n");
			return;
		}
		ret = avcodec_parameters_to_context(ist->dec_ctx, par);
		if (ret < 0) {
			OutputDebugString("Error initializing the decoder context.\n");
			return;
		}
		switch (par->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			if (!ist->dec)
				ist->dec = avcodec_find_decoder(par->codec_id);
			ist->dec_ctx->framerate = st->avg_frame_rate;
			in_cvinfo.videoindex = i;
			break;
		}
		ret = avcodec_parameters_from_context(par, ist->dec_ctx);
		if (ret < 0) {
			OutputDebugString("Error initializing the decoder context.\n");
			return;
		}
	}
	if (in_cvinfo.videoindex == -1) {
		sprintf(temp_buf, "Didn't find a video stream.\n");
		OutputDebugString(temp_buf);
	}
	
}
int open_input_file(const char *filename) 
{
	InputFile *f;
	AVFormatContext *ic;
	int err,i, ret;
	char temp_buf[255];

	ic = avformat_alloc_context();
	if (!ic) {
		OutputDebugString(" avformat_alloc_context error, ENOMEM");
		return -1;
	}
	err = avformat_open_input(&ic, filename, 0, 0);
	if (err < 0) {
		sprintf(temp_buf, "avformat_open_input error!\n");
		AfxMessageBox(temp_buf);
		return err;
	}
// 	for (i = 0; i < ic->nb_streams; i++) {
// 		avcodec_find_decoder(st->codecpar->codec_id);
// 	}
	in_cvinfo.pifmt_ctx = ic;
	ret = avformat_find_stream_info(ic, NULL);
	if (ret < 0) {
		sprintf(temp_buf, "%s: could not find codec parameters\n", filename);
		OutputDebugString(temp_buf);
		AfxMessageBox(temp_buf);
		if (ic->nb_streams == 0) {
			avformat_close_input(&ic);
			return -1;
		}
	}
	add_input_streams(ic);
	av_dump_format(ic, 0, filename, 0);
	f = (InputFile *)av_mallocz(sizeof(*f));
	if (!f) {
		OutputDebugString("insufficient memory, InputFile f not be allocate!\n");
		return -1;
	}
	f->ctx = ic;
	f->ist_index = nb_input_streams - ic->nb_streams;
	f->nb_streams = ic->nb_streams;

}

int CMFC_ffmpeg_streamerDlg::OpenInput(const char *pSrc = NULL)
{
	AVFormatContext *pifmt_ctx = NULL;
	int ret;
	int i;
	char temp_buf[255];
	if (pSrc == NULL)
		if (m_file_path != "") {
			pSrc = m_file_path;
		}

	open_input_file(pSrc);
	pifmt_ctx = in_cvinfo.pifmt_ctx;
	if (pifmt_ctx->duration != AV_NOPTS_VALUE) {
		int hours, mins, secs, us;
		int64_t duration = pifmt_ctx->duration + 5000;
		secs = duration / AV_TIME_BASE;
		in_cvinfo.video_secs = secs;
		us = duration % AV_TIME_BASE;
		mins = secs / 60;
		secs %= 60;
		hours = mins / 60;
		mins %= 60;
		sprintf(temp_buf, "%02d:%02d:%02d.%02d", hours, mins, secs, (100 * us) / AV_TIME_BASE);
		SetDlgItemText(IDC_STATIC_TOTALTIME, temp_buf);
	}
	// Setting slider
	m_ptsSlider.SetRange(0, in_cvinfo.video_secs);
	m_ptsSlider.SetTicFreq(1);
	
	/* 打印输入多媒体文件的信息 */
	av_dump_format(pifmt_ctx, 0, pSrc, 0);
	return 0;
}

int CMFC_ffmpeg_streamerDlg::CloseInput()
{
	AVFormatContext *pifmt_ctx = NULL;
	pifmt_ctx = in_cvinfo.pifmt_ctx;
	if(!pifmt_ctx)
		avformat_close_input(&pifmt_ctx);
	return 0;
}

int VideoPreview(CMFC_ffmpeg_streamerDlg *pDlg)
{
	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame, *pFrameYUV;
	unsigned char *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	//SDL---------------------------
	int screen_w = 0, screen_h = 0;
	SDL_Window *screen;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;

	FILE *fp_yuv;
	char tempbuf[255];
	char filepath[] = "cuc_ieschool.flv";
	pFormatCtx = in_cvinfo.pifmt_ctx;
	videoindex = in_cvinfo.videoindex;
	if (!pFormatCtx) return -1;
#if 0
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pFormatCtx, in_cvinfo.infile_name, NULL, NULL) != 0) {
		sprintf(tempbuf, "Couldn't open input stream.");
		AfxMessageBox(tempbuf);
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		sprintf(tempbuf,"Couldn't find stream information.");
		AfxMessageBox(tempbuf);
		return -1;
	}
	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		sprintf(tempbuf, "Didn't find a video stream.\n");
		AfxMessageBox(tempbuf);
		return -1;
	}
#endif


	//pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	//pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	pCodecCtx = avcodec_alloc_context3(NULL);
	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codecpar->codec_id);
	if (pCodec == NULL) {
		sprintf(tempbuf, "Codec not found.\n");
		AfxMessageBox(tempbuf);
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		sprintf(tempbuf, "Could not open codec.\n");
		AfxMessageBox(tempbuf);
		return -1;
	}
	screen = pDlg->sdl_win;
	sprintf(tempbuf, " win: 0x%p", screen);
	pDlg->SetStatusMessage(tempbuf);
	SDL_GetWindowSize(screen, &screen_w, &screen_h);
	SDL_Renderer * pRender = SDL_CreateRenderer(screen, -1, 0);// 0, SDL_RENDERER_ACCELERATED);
	//创建纹理
	SDL_Texture * pTexture = SDL_CreateTexture(pRender, SDL_PIXELFORMAT_IYUV /*SDL_PIXELFORMAT_YV12*/,
		SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
	
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	//Output Info-----------------------------
	//printf("--------------- File Information ----------------\n");
	//av_dump_format(pFormatCtx, 0, in_cvinfo.infile_name, 0);
	//printf("-------------------------------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

#if OUTPUT_YUV420P 
	fp_yuv = fopen("output.yuv", "wb+");
#endif  

	//sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	//sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;
	av_seek_frame(pFormatCtx, -1, in_cvinfo.seek_pos * AV_TIME_BASE , AVSEEK_FLAG_BACKWARD);//AVSEEK_FLAG_ANY);
	//SDL End----------------------
	//while (av_read_frame(pFormatCtx, packet) >= 0) {
	while(av_read_frame(pFormatCtx, packet)>= 0){
		if (packet->stream_index == videoindex) {
			/*ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				sprintf(tempbuf, "Decode Error.\n");
				AfxMessageBox(tempbuf);
				return -1;
			}*/
			ret = avcodec_send_packet(pCodecCtx, packet);
			got_picture = avcodec_receive_frame(pCodecCtx, pFrame);
			if (got_picture) {
				OutputDebugString("no frame could be decompressed, error!\n");
			} else {
				sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

#if OUTPUT_YUV420P
				y_size = pCodecCtx->width*pCodecCtx->height;
				fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y 
				fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
				fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
#endif
				//SDL---------------------------
#if 0
				SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
#else
				SDL_UpdateYUVTexture(pTexture, NULL,
					pFrameYUV->data[0], pFrameYUV->linesize[0],
					pFrameYUV->data[1], pFrameYUV->linesize[1],
					pFrameYUV->data[2], pFrameYUV->linesize[2]);
#endif	

				SDL_RenderClear(pRender);
				SDL_RenderCopy(pRender, pTexture, NULL, &sdlRect);
				SDL_RenderPresent(pRender);
				//SDL End-----------------------
				//Delay 40ms
				//SDL_Delay(40);
				break;
			}
		}
		av_packet_unref(packet);
	}
	av_packet_free(&packet);
#if 0
	//flush decoder
	//FIX: Flush Frames remained in Codec
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);
#if OUTPUT_YUV420P
		int y_size = pCodecCtx->width*pCodecCtx->height;
		fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y 
		fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
		fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
#endif
															//SDL---------------------------
		SDL_UpdateTexture(sdlTexture, &sdlRect, pFrameYUV->data[0], pFrameYUV->linesize[0]);
		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
		SDL_RenderPresent(sdlRenderer);
		//SDL End-----------------------
		//Delay 40ms
		SDL_Delay(40);
	}
#endif 
	av_free(out_buffer);
	sws_freeContext(img_convert_ctx);

#if OUTPUT_YUV420P 
	fclose(fp_yuv);
#endif 

	//SDL_Quit();
	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRender);
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avcodec_free_context(&pCodecCtx);
	//avformat_close_input(&pFormatCtx);

	return 0;
}

int VideoPreview2(int startTime, int endTime, const char *pSrc, const char *pDst)
{
	AVFormatContext *pifmt_ctx = NULL;
	AVFormatContext *pofmt_ctx = NULL;
	AVOutputFormat *pOutAVFmt = NULL;
	AVPacket pkt;
	int ret;
	int i;
	int isOpen = 0;
	char temp_buf[255];

	/* 打开输入多媒体文件 */
	if ((ret = avformat_open_input(&pifmt_ctx, pSrc, 0, 0)) < 0)
	{
		sprintf(temp_buf, "avformat_open_input error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}

	/* 打印输入多媒体文件的信息 */
	av_dump_format(pifmt_ctx, 0, pSrc, 0);

	/* 打开输出文件 */
	avformat_alloc_output_context2(&pofmt_ctx, NULL, NULL, pDst);
	if (!pofmt_ctx)
	{
		sprintf(temp_buf, "avformat_alloc_output_context2 error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	pOutAVFmt = pofmt_ctx->oformat;

	/* 为输出多媒体文件创建流并且拷贝流参数 */
	for (i = 0; i < pifmt_ctx->nb_streams; i++)
	{
		AVStream *pInStream = pifmt_ctx->streams[i];
		AVStream *pOutStream = avformat_new_stream(pofmt_ctx, NULL);
		if (!pOutStream)
		{
			sprintf(temp_buf, "avformat_new_stream error!\n");
			AfxMessageBox(temp_buf);
			logd(temp_buf);
			ret = AVERROR_UNKNOWN;
			goto end;
		}

		/* 拷贝参数 */
		//ret = avcodec_copy_context(pOutStream->codec, pInStream->codec);
		ret = avcodec_parameters_copy(pOutStream->codecpar, pInStream->codecpar);
		if (ret < 0)
		{
			sprintf(temp_buf, "avcodec_copy_context error!\n");
			AfxMessageBox(temp_buf);
			logd(temp_buf);
			goto end;
		}
		pOutStream->codecpar->codec_tag = 0;

	}

	av_dump_format(pofmt_ctx, 0, pDst, 1);

	/* 打开输出多媒体文件，准备写数据 */
	ret = avio_open(&pofmt_ctx->pb, pDst, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		sprintf(temp_buf, "avio_open error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}
	isOpen = 1;

	/* 写多媒体文件头 */
	ret = avformat_write_header(pofmt_ctx, NULL);
	if (ret < 0)
	{
		sprintf(temp_buf, "avformat_write_header error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}

	/* 移动到相应的时间点 */
	ret = av_seek_frame(pifmt_ctx, -1, startTime*AV_TIME_BASE, AVSEEK_FLAG_ANY);
	if (ret < 0)
	{
		sprintf(temp_buf, "av_seek_frame error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}

	int64_t *dtsStartTime = (int64_t *)malloc(sizeof(int64_t) * pifmt_ctx->nb_streams);
	memset(dtsStartTime, 0, sizeof(int64_t) * pifmt_ctx->nb_streams);
	int64_t *ptsStartTime = (int64_t *)malloc(sizeof(int64_t) * pifmt_ctx->nb_streams);
	memset(ptsStartTime, 0, sizeof(int64_t) * pifmt_ctx->nb_streams);

	while (1)
	{
		AVStream *pInStream, *pOutStream;

		ret = av_read_frame(pifmt_ctx, &pkt);
		if (ret < 0)
			break;

		pInStream = pifmt_ctx->streams[pkt.stream_index];
		pOutStream = pofmt_ctx->streams[pkt.stream_index];

		if (av_q2d(pInStream->time_base) * pkt.pts > endTime)
		{
			av_packet_unref(&pkt);
			break;
		}

		if (dtsStartTime[pkt.stream_index] == 0)
			dtsStartTime[pkt.stream_index] = pkt.dts;

		if (ptsStartTime[pkt.stream_index] == 0)
			ptsStartTime[pkt.stream_index] = pkt.pts;

		/* 转化时间基 */
		if(pkt.pts != AV_NOPTS_VALUE)
			pkt.pts = av_rescale_q_rnd(pkt.pts - ptsStartTime[pkt.stream_index], pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		if (pkt.dts != AV_NOPTS_VALUE)
			pkt.dts = av_rescale_q_rnd(pkt.dts - dtsStartTime[pkt.stream_index], pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		if (pkt.pts < 0)
			pkt.pts = 0;

		if (pkt.dts < 0)
			pkt.dts = 0;
		if(pkt.duration > 0)
			pkt.duration = (int)av_rescale_q((int64_t)pkt.duration, pInStream->time_base, pOutStream->time_base);
		pkt.pos = -1;

		/* 写数据 */
		ret = av_interleaved_write_frame(pofmt_ctx, &pkt);
		if (ret < 0)
		{
			sprintf(temp_buf, "av_interleaved_write_frame error!\n");
			AfxMessageBox(temp_buf);
			logd(temp_buf);
			break;
		}

		av_packet_unref(&pkt);
	}

	free(dtsStartTime);
	free(ptsStartTime);

	av_write_trailer(pofmt_ctx);

end:
	if (isOpen)
		avio_closep(&pofmt_ctx->pb);

	if (pifmt_ctx)
		avformat_close_input(&pifmt_ctx);

	if (pofmt_ctx)
		avformat_free_context(pofmt_ctx);

	return ret;
}


/*
* startTime start time in microseconds 
*/
int cutVideo(CMFC_ffmpeg_streamerDlg *dlg, int64_t startTime, int64_t endTime, const char *pSrc, const char *pDst)
{
	AVFormatContext *pifmt_ctx = NULL;
	AVFormatContext *pofmt_ctx = NULL;
	AVOutputFormat *pOutAVFmt = NULL;
	OutputStream *ost;
	OutputFile *of;

	AVPacket pkt;
	AVPacket opkt;
	AVCodec *pInCodec;
	int ret;
	int i;
	int frame_cnt=0;
	int isOpen = 0;
	char temp_buf[255];
	int64_t recording_time;

	if (startTime > endTime) {
		OutputDebugString("error, startTime > endTime");
		AfxMessageBox("Not allow to this operation, because the start time larger than the end of time");
		return -1;
	}
	if (!(ost = (OutputStream *)av_mallocz(sizeof(*ost)))) {
		OutputDebugString("insufficient Memory, can not allocate OutputStream *ost!\n");
		return -1;
	}
	
	if (!(of = (OutputFile *)av_mallocz(sizeof(*of)))) {
		OutputDebugString("insufficient Memory, can not allocate OutputFile *of!\n");
		return -1;
	}

		recording_time = endTime - startTime;
#if 0
	/* 打开输入多媒体文件 */
	if ((ret = avformat_open_input(&pifmt_ctx, pSrc, 0, 0)) < 0)
	{
		sprintf(temp_buf, "avformat_open_input error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}

	/* 打印输入多媒体文件的信息 */
	av_dump_format(pifmt_ctx, 0, pSrc, 0);
#endif 
	pifmt_ctx = in_cvinfo.pifmt_ctx;
	/* 打开输出文件 */
	ret = avformat_alloc_output_context2(&pofmt_ctx, NULL, NULL, pDst);
	if (!pofmt_ctx)
	{
		sprintf(temp_buf, "avformat_alloc_output_context2 error, errno no %d!\n", ret);
		OutputDebugString(temp_buf);
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	pOutAVFmt = pofmt_ctx->oformat;

	/* 为输出多媒体文件创建流并且拷贝流参数 */
	for (i = 0; i < pifmt_ctx->nb_streams; i++)
	{
		AVStream *pInStream = pifmt_ctx->streams[i];
		AVStream *pOutStream = avformat_new_stream(pofmt_ctx, NULL);
		if (!pOutStream)
		{
			sprintf(temp_buf, "avformat_new_stream error!\n");		
			OutputDebugString(temp_buf);
			AfxMessageBox(temp_buf);
			logd(temp_buf);
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		/* 拷贝参数 */
		ret = avcodec_parameters_copy(pOutStream->codecpar, pInStream->codecpar);
		if (ret < 0)
		{
			printf("avcodec_parameters_copy error!\n");
			OutputDebugString(temp_buf);
			AfxMessageBox(temp_buf);	
			logd(temp_buf);
			goto end;
		}

		pOutStream->codecpar->codec_tag = 0;

	}

	av_dump_format(pofmt_ctx, 0, pDst, 1);

	/* 打开输出多媒体文件，准备写数据 */
	ret = avio_open(&pofmt_ctx->pb, pDst, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		sprintf(temp_buf, "avio_open error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}
	isOpen = 1;
	av_init_packet(&pkt);
	av_init_packet(&opkt);
	/* 写多媒体文件头 */
	ret = avformat_write_header(pofmt_ctx, NULL);
	if (ret < 0)
	{
		sprintf(temp_buf, "avformat_write_header error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}

	/* 移动到相应的时间点 */
	ret = av_seek_frame(pifmt_ctx, -1, startTime, AVSEEK_FLAG_ANY);
	if (ret < 0)
	{
		sprintf(temp_buf, "av_seek_frame error!\n");
		AfxMessageBox(temp_buf);
		logd(temp_buf);
		goto end;
	}

	int64_t *dtsStartTime = (int64_t *)malloc(sizeof(int64_t) * pifmt_ctx->nb_streams);
	memset(dtsStartTime, 0, sizeof(int64_t) * pifmt_ctx->nb_streams);
	int64_t *ptsStartTime = (int64_t *)malloc(sizeof(int64_t) * pifmt_ctx->nb_streams);
	memset(ptsStartTime, 0, sizeof(int64_t) * pifmt_ctx->nb_streams);
	int64_t ost_tb_start_time = av_rescale_q(startTime, AV_TIME_BASE_Q1, ost->mux_timebase);
	while (1)
	{
		AVStream *pInStream, *pOutStream;

		ret = av_read_frame(pifmt_ctx, &pkt);
		if (ret < 0)
			break;
		frame_cnt++;
		sprintf(temp_buf, "stream_index = %d\n", pkt.stream_index);
		OutputDebugString(temp_buf);
		sprintf(temp_buf, "process frame %d", frame_cnt);
		dlg->SetStatusMessage(temp_buf);
		pInStream = pifmt_ctx->streams[pkt.stream_index];
		pOutStream = pofmt_ctx->streams[pkt.stream_index];
		ost->mux_timebase = pOutStream->time_base;
		//if (av_q2d(pInStream->time_base) * pkt.pts > endTime)
		if (pkt.pts >= recording_time + startTime)
		{
			//av_free_packet(&pkt);
			av_packet_unref(&pkt);
			break;
		}
		
		//将截取后的每个流的起始dts、pts保存下来，作为开始时间，用来做后面的时基转换
		if (dtsStartTime[pkt.stream_index] == 0) {
			dtsStartTime[pkt.stream_index] = pkt.dts;
			sprintf(temp_buf, "frame[%d] dts_start_from: %lld\n", frame_cnt, dtsStartTime[pkt.stream_index]);
			OutputDebugString(temp_buf);
		}

		if (ptsStartTime[pkt.stream_index] == 0) {
			ptsStartTime[pkt.stream_index] = pkt.pts;
			sprintf(temp_buf, "frame[%d] pts_start_from: %lld\n", frame_cnt, ptsStartTime[pkt.stream_index]);
			OutputDebugString(temp_buf);
		}
		sprintf(temp_buf, "frame[%d]  111>>> pts = %lld, dts = %lld\n", frame_cnt, pkt.pts, pkt.dts);
		OutputDebugString(temp_buf);
		//
		
		if (pkt.pts != AV_NOPTS_VALUE)
			opkt.pts = av_rescale_q(pkt.pts, pInStream->time_base, ost->mux_timebase) - ost_tb_start_time;
		else
			opkt.pts = AV_NOPTS_VALUE;
		/*if (pkt.dts == AV_NOPTS_VALUE)
			opkt.dts = av_rescale_q(ist->dts, AV_TIME_BASE_Q1, ost->mux_timebase);
		else*/
			opkt.dts = av_rescale_q(pkt.dts, pInStream->time_base, ost->mux_timebase);
		opkt.dts -= ost_tb_start_time;
		
		/* 转化时间基 */
		/*pkt.pts = av_rescale_q_rnd(pkt.pts - ptsStartTime[pkt.stream_index], pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts - dtsStartTime[pkt.stream_index], pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		if (pkt.pts < 0)
			pkt.pts = 0;

		if (pkt.dts < 0)
			pkt.dts = 0;
		
		pkt.duration = (int)av_rescale_q((int64_t)pkt.duration, pInStream->time_base, pOutStream->time_base);
		pkt.pos = -1;
		*/
		opkt.duration = av_rescale_q(pkt.duration, pInStream->time_base, ost->mux_timebase);
		opkt.flags = pkt.flags;

		opkt.data = pkt.data;
		opkt.size = pkt.size;
		//av_copy_packet_side_data(&opkt, &pkt);

		sprintf(temp_buf, " 222>>> pts = %lld, dts = %lld\n", pkt.pts, pkt.dts);
		OutputDebugString(temp_buf);
		//if (pkt.pts < pkt.dts) { continue; }
		/* 写数据 */
		ret = av_interleaved_write_frame(pofmt_ctx, &pkt);
		if (ret < 0)
		{
			sprintf(temp_buf, "av_interleaved_write_frame error! ret = %d\n", ret);
			OutputDebugString(temp_buf);
			AfxMessageBox(temp_buf);
			logd(temp_buf);
			sprintf(temp_buf, "[av_interleaved_write_frame]  pts = %lld, dts = %lld\n", pkt.pts, pkt.dts);
			OutputDebugString(temp_buf);
			break;
		}
		av_packet_unref(&pkt);
		av_packet_unref(&opkt);
	}

	free(dtsStartTime);
	free(ptsStartTime);

	av_write_trailer(pofmt_ctx);

end:
	if (isOpen)
		avio_closep(&pofmt_ctx->pb);


	if (pifmt_ctx)
		avformat_close_input(&pifmt_ctx);

	if (pofmt_ctx)
		avformat_free_context(pofmt_ctx);

	return ret;
}

UINT ThreadPlayer(LPVOID lparam)
{
	CMFC_ffmpeg_streamerDlg *filterDlg = (CMFC_ffmpeg_streamerDlg *)lparam;
	mffmpeg_player(filterDlg);
	//push_streamer(filterDlg);
	//getcapdev_push_streamer(filterDlg);
	return 0;
}

void CMFC_ffmpeg_streamerDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码

	AfxBeginThread(ThreadPlayer, this);
}


void CMFC_ffmpeg_streamerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CTime t = CTime::GetCurrentTime();
	m_statusbar.SetPaneText(1, t.Format(_T("%Y%m%d %H:%M:%S"))); //星期%w
	CDialogEx::OnTimer(nIDEvent);
}


int  CMFC_ffmpeg_streamerDlg::SetStatusMessage(char *buf)
{
	//m_statusbar.SetPaneText(0, _T("chifan"));
	SendMessage(WM_USER_MSG1, 0, (LPARAM)buf);
	return 0;
}


afx_msg LRESULT CMFC_ffmpeg_streamerDlg::OnUserMsg1(WPARAM wParam, LPARAM lParam)
{
	
	m_statusbar.SetPaneText(0, (LPCTSTR)lParam);
	return 0;
}


void CMFC_ffmpeg_streamerDlg::OnOpenFile()
{
	// TODO: 在此添加命令处理程序代码
	int i = 0;
	BOOL isOpen = TRUE;		//是否打开(否则为保存)
	CString defaultDir = ".\\";	//默认打开的文件路径
	CString fileName = "";			//默认打开的文件名
	CString filter = "文件 (*.*)|*.*||";	//文件过虑的类型
	LV_ITEM lvitem;
	char buf[255] = {0};
	int flag_exist = 0;
	int pos;
	int len;
	GetModuleFileName(NULL, defaultDir.GetBuffer(MAX_PATH), MAX_PATH);
	defaultDir.ReleaseBuffer();
	pos = defaultDir.ReverseFind(_T('\\'));
	len = defaultDir.GetLength();
	defaultDir = defaultDir.Left(pos);
	//AfxMessageBox(defaultDir);
	defaultDir = "E:\\xxwork\\testvideo\\*.*";
	CFileDialog openFileDlg(isOpen, 0, defaultDir, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = ".\\";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + "\\..\\mfc_ffmpeg_streamer\\cuc_ieschool.flv";
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	m_file_path = filePath;
	OpenInput();
	SetWindowText(filePath);
	for (i = 0; i < m_playListCtrl.GetItemCount(); i++) {
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.mask = LVIF_TEXT;
		lvitem.pszText = buf;
		lvitem.cchTextMax = 1024;
		m_playListCtrl.GetItem(&lvitem);
		if (filePath.CompareNoCase(buf) == 0) {
			flag_exist = 1;
			break;
		}
	}
	if (!flag_exist) {
		pos = filePath.ReverseFind('\\');
		CString afileName =filePath.Right(filePath.GetLength() - pos-1);

		m_playListCtrl.InsertItem(m_playListCtrl.GetItemCount(), afileName, NULL);
	}
	SendMessage(WM_NCPAINT, NULL, NULL);

}


LRESULT CMFC_ffmpeg_streamerDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	//return CDialogEx::DefWindowProc(message, wParam, lParam);
	//LRESULT lResult = CDialogEx::DefWindowProc(message, wParam, lParam);

	LRESULT lResult = CDialogEx::DefWindowProc(message, wParam, lParam);
	if (!::IsWindow(m_hWnd))
		return lResult;
	if (message == WM_NCPAINT || message == WM_NCACTIVATE || message == WM_NOTIFY || message == WM_MOVE)
	{
		CDC *pDC = GetWindowDC();
		CRect RectWnd, RectTitle, RectButtons;
		GetWindowRect(&RectWnd);
		//GetClientRect(&RectWnd);
		int edge = GetSystemMetrics(SM_CYFIXEDFRAME);
		int frame_border = GetSystemMetrics(SM_CYFIXEDFRAME);
		int dlg_border = frame_border + edge;
		
		//CString str;
		//str.Format("%d", GetSystemMetrics(SM_CYFIXEDFRAME));// 对话框边框厚度
		//AfxMessageBox(str);
		//取得整个标题栏的矩形
		RectTitle.left = GetSystemMetrics(SM_CXFRAME) + frame_border ;
		RectTitle.top = GetSystemMetrics(SM_CYFRAME) + frame_border;
		RectTitle.right = RectWnd.right - RectWnd.left - frame_border -2*edge;//+ dlg_border+2;// -
			//GetSystemMetrics(SM_CXFRAME);
		RectTitle.bottom = RectTitle.top + GetSystemMetrics(SM_CYSIZE);
		//重画标题栏
		CBrush brush;
		brush.CreateSolidBrush(RGB(0, 120, 200));
		pDC->FillRect(RectTitle, &brush);
		

		//重画图标
		HICON hIcon = m_hIcon;//(HICON)::GetClassLong(m_hWnd, GCL_HICON);
		m_SaveIcon.left = RectTitle.left + dlg_border; //RectTitle.right - GetSystemMetrics(SM_CYSMICON);
		m_SaveIcon.top = RectTitle.top + dlg_border;
		m_SaveIcon.right = m_SaveIcon.left + GetSystemMetrics(SM_CXSMICON);
		m_SaveIcon.bottom = m_SaveIcon.top + GetSystemMetrics(SM_CYSMICON);
		::DrawIconEx(pDC->m_hDC, m_SaveIcon.left, m_SaveIcon.top, hIcon,
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
			0, NULL, DI_NORMAL);

		//记录图标屏幕位置
		m_SaveIcon.OffsetRect(RectWnd.TopLeft());

		//重画最小化按钮
		int nButtHeight = GetSystemMetrics(SM_CYSMSIZE) - frame_border;
		int nButtWidth = GetSystemMetrics(SM_CYSMSIZE);
		RectButtons.left = RectTitle.right - nButtWidth *3  ;
		RectButtons.top = RectTitle.top +
			(GetSystemMetrics(SM_CYSIZE) - nButtHeight) / 2 ;
		RectButtons.right = RectButtons.left + nButtWidth;
		RectButtons.bottom = RectButtons.top + nButtHeight;
		pDC->DrawFrameControl(&RectButtons, DFC_CAPTION, DFCS_CAPTIONMIN);
		m_SaveButtonMin = RectButtons;
		//记录最小化按钮屏幕位置
		m_SaveButtonMin.OffsetRect(RectWnd.TopLeft());
		//重画最大化或恢复按钮
		RectButtons.left = RectButtons.right;
		RectButtons.right = RectButtons.left + nButtWidth;

		pDC->DrawFrameControl(&RectButtons, DFC_CAPTION, DFCS_CAPTIONMIN);
		pDC->DrawFrameControl(&RectButtons, DFC_CAPTION, IsZoomed() ?
			DFCS_CAPTIONRESTORE : DFCS_CAPTIONMAX);
		m_SaveButtonMax = RectButtons;
		//记录最大化按钮屏幕位置
		m_SaveButtonMax.OffsetRect(RectWnd.TopLeft());

		//重画关闭按钮
		RectButtons.left = RectButtons.right;
		RectButtons.right = RectButtons.left + nButtWidth;
		pDC->DrawFrameControl(&RectButtons, DFC_CAPTION, DFCS_CAPTIONCLOSE);
		m_SaveButtonClose = RectButtons;
		//记录关闭按钮屏幕位置
		m_SaveButtonClose.OffsetRect(RectWnd.TopLeft());
		//重画标题栏
		int nOldMode = pDC->SetBkMode(TRANSPARENT);
		COLORREF clOldText = pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SelectStockObject(ANSI_FIXED_FONT);
		//RectTitle.right -= GetSystemMetrics(SM_CYSMICON);
		CString title;
		GetWindowText(title.GetBuffer(255), 255);
		RectTitle.top += 5;
		pDC->DrawText(title, -1, &RectTitle, DT_CENTER);
		pDC->SetBkMode(nOldMode);
		pDC->SetTextColor(clOldText);
	}
	return lResult;
}


BOOL CMFC_ffmpeg_streamerDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	// 样式
	
	//return CDialogEx::PreCreateWindow(cs);
	if (!CDialogEx::PreCreateWindow(cs))
		return FALSE;
	cs.style &= ~WS_SYSMENU; //取消标题栏上的按钮
	return TRUE;
}


void CMFC_ffmpeg_streamerDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	
	//检测最小，最大和关闭按钮是否按到
	
	CDialogEx::OnNcLButtonDown(nHitTest, point);
	//CString str;
	//str.Format("%d, %d", point.x, point.y);
	//AfxMessageBox(str);
	//str.Format("close icon (%d,%d), (%d,%d)", m_SaveButtonClose.left, m_SaveButtonClose.top,
	//	m_SaveButtonClose.right, m_SaveButtonClose.bottom);
	//AfxMessageBox(str);
	if (m_SaveButtonClose.PtInRect(point)) {
		SendMessage(WM_CLOSE);
	}
	else if (m_SaveButtonMin.PtInRect(point))
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, MAKELPARAM(point.x, point.y));
	else if (m_SaveButtonMax.PtInRect(point))
	{
		if (IsZoomed())
			SendMessage(WM_SYSCOMMAND, SC_RESTORE,
				MAKELPARAM(point.x, point.y));
		else
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE,
				MAKELPARAM(point.x, point.y));
	}
}
void secstime_to_text(int secs, char *buf) {
	int hours, mins;
	char temp_buf[10];
	mins = secs / 60;
	secs %= 60;
	hours = mins / 60;
	mins %= 60;
	sprintf(temp_buf, "%02d:%02d:%02d", hours, mins, secs);
	strcpy(buf, temp_buf);
}
void CMFC_ffmpeg_streamerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int hours, mins, secs;
	char temp_buf[10];
	
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_PTS);
	//CString str;
	//str.Format("%p %p", pScrollBar, pSlider);
	//AfxMessageBox(str);
// 	if (nSBCode == SB_THUMBPOSITION) {
// 		CString str;
// 		str.Format("%p %d", pScrollBar, nPos);
// 		AfxMessageBox(str);
// 	}
// 	secs = pSlider->GetPos();
// 	mins = secs / 60;
// 	secs %= 60;
// 	hours = mins / 60;
// 	mins %= 60;
// 	sprintf(temp_buf, "%02d:%02d:%02d", hours, mins, secs);
// 	SetDlgItemText(IDC_STATIC_CURTIME, temp_buf);
	//


	int nID = pScrollBar->GetDlgCtrlID();    //获取滑动块ID号
	if (nID == IDC_SLIDER_PTS){
		switch (nSBCode) {                    // 按是哪一种操作进行处理
		case SB_LINELEFT:
		    //点击左箭头
			break;
		case SB_LINERIGHT: 
			 //点击右箭头
			break;
		case SB_THUMBPOSITION:               //拖动滑动块（不允许拖动）
		
			break;
		case SB_ENDSCROLL:		
			break;
		}
		secs = pSlider->GetPos();
		in_cvinfo.seek_pos = secs;
		secstime_to_text(secs, temp_buf);
		//AfxMessageBox(temp_buf);
		SetDlgItemText(IDC_STATIC_CURTIME, temp_buf);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMFC_ffmpeg_streamerDlg::OnNMCustomdrawSliderPts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CMFC_ffmpeg_streamerDlg::OnReleasedcaptureSliderPts(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	
	VideoPreview(this);
	*pResult = 0;
}


void CMFC_ffmpeg_streamerDlg::OnClickedMovieClip()
{
	//char temp[255];
	if (movclip.sv_filepath[0] == '\0') {
#if 0
		AfxMessageBox("please input save path");
		return;
#else
		OutputDebugString("no file name was entered,use default E:\\xxwork\\test.mp4\n");
		strcpy(movclip.sv_filepath, "E:\\xxwork\\test.mp4");
#endif
	}
	// TODO: 在此添加控件通知处理程序代码
	cutVideo(this, movclip.start_time * AV_TIME_BASE, movclip.end_time * AV_TIME_BASE, in_cvinfo.infile_name, movclip.sv_filepath);
}


void CMFC_ffmpeg_streamerDlg::OnBnClickedBtnCutstart()
{
	// TODO: 在此添加控件通知处理程序代码
	char temp_buf[10];
	movclip.start_time = in_cvinfo.seek_pos;
	secstime_to_text(movclip.start_time, temp_buf);
	SetDlgItemText(IDC_STATIC_CUTSTART, temp_buf);
}


void CMFC_ffmpeg_streamerDlg::OnBnClickedBtnCutend()
{
	// TODO: 在此添加控件通知处理程序代码
	char temp_buf[10];
	movclip.end_time = in_cvinfo.seek_pos;
	secstime_to_text(movclip.end_time, temp_buf);
	SetDlgItemText(IDC_STATIC_CUTEND, temp_buf);
}




void CMFC_ffmpeg_streamerDlg::OnClickedBtnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL isOpen = FALSE;	//是否打开(否则为保存)
	CString defaultDir = "E:\\";	//默认打开的文件路径
	CString fileName = "test.avi";	//默认打开的文件名
	CString filter = "文件 (*.*)|*.*||";	//文件过虑的类型
	CFileDialog openFileDlg(isOpen, 0, defaultDir + fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	openFileDlg.GetOFN().lpstrInitialDir = "E:\\test.avi";
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + fileName;
	if (result == IDOK) {
		filePath = openFileDlg.GetPathName();
	}
	SetDlgItemText(IDC_EDIT_SAVEFILE, filePath);
	strcpy(movclip.sv_filepath, filePath.GetBuffer(filePath.GetLength()));
}


void CMFC_ffmpeg_streamerDlg::OnBnClickedBtnCon()
{
	// TODO: 在此添加控件通知处理程序代码
	CConcatDlg *dlg = new CConcatDlg();
	//dlg->DoModal();
	dlg->Create(IDD_CONCAT);
	dlg->ShowWindow(SW_SHOW);
}
