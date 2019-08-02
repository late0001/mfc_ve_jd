
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
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_MSG1, &CMFC_ffmpeg_streamerDlg::OnUserMsg1)
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
	if (buf == NULL) return -1;

	FILE *plogfile = fopen(pSzLogFileName, "ab");
	fwrite(buf, sizeof(char), strlen(buf), plogfile);
	//fflush(plogfile);
	fclose(plogfile);
	return 0;
}
BOOL CMFC_ffmpeg_streamerDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
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
	FILE *plogfile = fopen(pSzLogFileName, "wb+");
	fclose(plogfile);
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
		if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
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
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		if (!out_stream) {
			AfxMessageBox("Failed allocating output stream");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//复制AVCodecContext的设置（Copy the settings of AVCodecContext）
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			AfxMessageBox("Failed to copy context from input to output stream codec context");
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
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
			AVRational time_base_q = { 1,AV_TIME_BASE };
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

		av_free_packet(&pkt);

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
	AVCodecContext *pCodecCtx = NULL;
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

	in_filename = "cuc_ieschool.flv";//输入URL（Input file URL）

	out_filename = "rtmp://192.168.0.104:1935/cctvf/zm";//输出 URL（Output URL）[RTMP]
	//out_filename = "rtp://233.233.233.233:6666";//输出 URL（Output URL）[UDP]
	FILE *plogfile = fopen(pSzLogFileName, "wb+");
	fclose(plogfile);
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
		if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		AfxMessageBox("Couldn't find a video stream.(没有找到视频流)");
		goto end;
	}
	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	//输出（Output）
	pdeCodex = avcodec_find_decoder(ifmt_ctx->streams[videoindex]->codec->codec_id);
	if(pdeCodex == NULL){
		AfxMessageBox("Couldn't find decoder.");
		goto end;
	}
	if (avcodec_open2(ifmt_ctx->streams[videoindex]->codec, pdeCodex, NULL) < 0) {
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
	pCodecCtx->pix_fmt = PIX_FMT_YUV420P;
	pCodecCtx->width = ifmt_ctx->streams[videoindex]->codec->width;
	pCodecCtx->height = ifmt_ctx->streams[videoindex]->codec->height;
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
#if 0
	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		//根据输入流创建输出流（Create output AVStream according to input AVStream）
		AVStream *in_stream = ifmt_ctx->streams[i];
		AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		if (!out_stream) {
			AfxMessageBox("Failed allocating output stream");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//复制AVCodecContext的设置（Copy the settings of AVCodecContext）
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			AfxMessageBox("Failed to copy context from input to output stream codec context");
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	}
#endif
	AVStream *video_st = avformat_new_stream(ofmt_ctx, penCodec);
	if (!video_st) {
		AfxMessageBox("Failed allocating video stream");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	video_st->time_base.num = 1;
	video_st->time_base.den = 25;
	video_st->codec = pCodecCtx;

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
	img_convert_ctx = sws_getContext(ifmt_ctx->streams[videoindex]->codec->width, ifmt_ctx->streams[videoindex]->codec->height,
		ifmt_ctx->streams[videoindex]->codec->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	pFrameYUV = av_frame_alloc();
	uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

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
		ret = avcodec_decode_video2(ifmt_ctx->streams[dec_pkt->stream_index]->codec, pFrame,
			&dec_got_frame, dec_pkt);
		if (ret < 0) {
			av_frame_free(&pFrame);
			av_log(NULL, AV_LOG_ERROR, "Decoding failed\n");
			break;
		}
		if (dec_got_frame) {
			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

			enc_pkt.data = NULL;
			enc_pkt.size = 0;
			av_init_packet(&enc_pkt);
			ret = avcodec_encode_video2(pCodecCtx, &enc_pkt, pFrameYUV, &enc_got_frame);
			av_frame_free(&pFrame);
			if (enc_got_frame == 1) {
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
				av_free_packet(&enc_pkt);
			}
		}
		else {
			av_frame_free(&pFrame);
		}
		av_free_packet(dec_pkt);

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

int mffmpeg_player(CMFC_ffmpeg_streamerDlg *filterDlg)
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
				break;//continue;//
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
	filterDlg->SetStatusMessage("close input file");
	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRender);
	//让控制线程退出
	thread_exit = 1;
	int status = 0;
	if(video_tid > 0)
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
	return 0;
}

UINT ThreadPlayer(LPVOID lparam)
{
	CMFC_ffmpeg_streamerDlg *filterDlg = (CMFC_ffmpeg_streamerDlg *)lparam;
	//mffmpeg_player(filterDlg);
	//push_streamer(filterDlg);
	getcapdev_push_streamer(filterDlg);
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
