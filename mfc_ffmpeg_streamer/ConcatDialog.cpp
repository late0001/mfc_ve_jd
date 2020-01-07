// ConcatDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_ffmpeg_streamer.h"
#include "ConcatDialog.h"
#include "afxdialogex.h"
#include <list>
using namespace std;

// CConcatDialog 对话框

IMPLEMENT_DYNAMIC(CConcatDlg, CDialogEx)

CConcatDlg::CConcatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONCAT, pParent)
{

}

CConcatDlg::~CConcatDlg()
{
}

void CConcatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTFILE, m_mflist);
	DDX_Control(pDX, IDC_EDIT_FILEN, m_edit1);
}


BEGIN_MESSAGE_MAP(CConcatDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CONVD, &CConcatDlg::OnBnClickedBtnConvd)
	ON_NOTIFY(NM_CLICK, IDC_LISTFILE, &CConcatDlg::OnClickListfile)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTFILE, &CConcatDlg::OnDblclkListfile)
    ON_STN_CLICKED(IDC_ADDLVIM, &CConcatDlg::OnClickedAddlvim)
    ON_NOTIFY(NM_RCLICK, IDC_LISTFILE, &CConcatDlg::OnRclickListfile)
    ON_COMMAND(ID_32779, &CConcatDlg::OnMoveDown)
	ON_BN_CLICKED(IDC_BUTTON1, &CConcatDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CConcatDialog 消息处理程序


BOOL CConcatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	DWORD dwstyle = m_mflist.GetExtendedStyle();
	dwstyle &= ~LVS_TYPEMASK;
	dwstyle |= LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
	m_mflist.SetExtendedStyle( dwstyle);
	//LONG lstyle = GetWindowLong(m_mflist.m_hWnd, GWL_STYLE);
	//SetWindowLong(m_mflist.m_hWnd, GWL_STYLE, lstyle | LVS_REPORT);
	m_mflist.InsertColumn(0, "File", LVCFMT_LEFT, 300, -1);
	m_mflist.InsertColumn(1, " ", LVCFMT_LEFT, 100, -1);
	int nlow = m_mflist.InsertItem(0, "hello world");
	m_mflist.SetItemText(nlow, 1, "...");

	nlow = m_mflist.InsertItem(1, "hello1");
	m_mflist.SetItemText(nlow, 1, "...");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CConcatDlg::OnBnClickedBtnConvd()
{
	// TODO: 在此添加控件通知处理程序代码
	
	struct ab a1, a2, a3;
	a1.a = 2;
	a1.b = 5;
	strcpy(a1.name, "hello");
	la.push_back(a1);
	a2.a = 3;
	a2.b = 4;
	strcpy(a2.name, "world");
	la.push_back(a2);
	a3.a = 7;
	a3.b = 5;
	strcpy(a3.name, "hello kitty");
	la.push_back(a3);
	int j = 2;
	list<struct ab>::iterator i;
	for (i = la.begin(); i != la.end(); ++i){
		m_mflist.InsertItem(m_mflist.GetItemCount(), i->name);
	}

}


void CConcatDlg::OnClickListfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	/*CString str;
	int row = m_mflist.GetNextItem(-1, LVNI_SELECTED);
	str.Format("%d", row);
	AfxMessageBox(str);*/
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	int nRow = pNMListView->iItem;
	int nCol = pNMListView->iSubItem;
	//CString str;
	//str.Format("%d, %d", nRow, nCol);
	//AfxMessageBox(str);
	if (nCol == 1 && nRow != -1) //如果选择的是子项并且主项不为空;
	{
		m_edit1.ShowWindow(SW_HIDE);
		int i = 0;
		BOOL isOpen = TRUE;		//是否打开(否则为保存)
		CString defaultDir = ".\\";	//默认打开的文件路径
		CString fileName = "";			//默认打开的文件名
		CString filter = "文件 (*.*)|*.*||";	//文件过虑的类型
		char buf[255] = { 0 };
		int pos;
		int len;
		GetModuleFileName(NULL, defaultDir.GetBuffer(MAX_PATH), MAX_PATH);
		defaultDir.ReleaseBuffer();
		pos = defaultDir.ReverseFind(_T('\\'));
		len = defaultDir.GetLength();
		defaultDir = defaultDir.Left(pos);
		//AfxMessageBox(defaultDir);
		CFileDialog openFileDlg(isOpen, 0, defaultDir, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
		openFileDlg.GetOFN().lpstrInitialDir = ".\\";
		INT_PTR result = openFileDlg.DoModal();
		CString filePath = defaultDir + "\\..\\mfc_ffmpeg_streamer\\cuc_ieschool.flv";
		if (result == IDOK) {
			filePath = openFileDlg.GetPathName();
		}
		m_mflist.SetItemText(nRow, 0, filePath);


	}
	*pResult = 0;
}


void CConcatDlg::OnDblclkListfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	int nRow = pNMListView->iItem;
	int nCol = pNMListView->iSubItem;

	//CString str;
	//str.Format("%d, %d", nRow, nCol);
	//AfxMessageBox(str);
	if (nCol == 0 ) //如果选择的是非子项;
	{
		
		m_mflist.GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);//获得子项的RECT；
		m_edit1.SetParent(&m_mflist);//转换坐标为列表框中的坐标
		m_edit1.MoveWindow(rc);//移动Edit到RECT坐在的位置;
		m_edit1.SetWindowText(m_mflist.GetItemText(nRow, nCol));//将该子项中的值放在Edit控件中；
		m_edit1.ShowWindow(SW_SHOW);//显示Edit控件；
		m_edit1.SetFocus();//设置Edit焦点
		//m_edit1.ShowCaret();//显示光标
		m_edit1.SetSel(0,-1);//将光标移动到最后
		m_edit1.ShowWindow(SW_SHOW);

	}

	*pResult = 0;
}


void CConcatDlg::OnClickedAddlvim()
{
	// TODO: 在此添加控件通知处理程序代码
		int icnt;
		icnt = m_mflist.GetItemCount();
		m_mflist.InsertItem(icnt, "");
		m_mflist.SetItemText(icnt, 1, "...");
}


void CConcatDlg::OnRclickListfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu menu;
	menu.LoadMenu(IDR_MENU2);      //加载菜单资源
	CMenu* pPopup = menu.GetSubMenu(0);   //获得次级菜单
	CPoint myPoint;
	GetCursorPos(&myPoint);      //获取鼠标坐标							
	pPopup->TrackPopupMenu(TPM_LEFTALIGN, myPoint.x, myPoint.y, this); //弹出菜单

	*pResult = 0;
}


void CConcatDlg::OnMoveDown() //下移动 菜单
{
	// TODO: 在此添加命令处理程序代码

}

int concatVideo(/*CMFC_ffmpeg_streamerDlg *dlg ,*/ int startTime, int endTime, const char *pSrc, const char *pSrc1, const char *pDst)
{
	AVFormatContext *pifmt_ctx = NULL;
	AVFormatContext *pofmt_ctx = NULL;
	AVOutputFormat *pOutAVFmt = NULL;
	AVPacket pkt;
	int ret;
	int i;
	int frame_cnt = 0;
	int frame_cnt_total = 0;
	int isOpen = 0;
	char temp_buf[255];
	int64_t last_pts = 0, last_dts = 0;
#if 1
	/* 打开输入多媒体文件 */
	if ((ret = avformat_open_input(&pifmt_ctx, pSrc, 0, 0)) < 0)
	{
		sprintf(temp_buf, "avformat_open_input error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		goto end;
	}

	/* 打印输入多媒体文件的信息 */
	av_dump_format(pifmt_ctx, 0, pSrc, 0);
#endif 
	//pifmt_ctx = in_cvinfo.pifmt_ctx;
	/* 打开输出文件 */
	avformat_alloc_output_context2(&pofmt_ctx, NULL, NULL, pDst);
	if (!pofmt_ctx)
	{
		sprintf(temp_buf, "avformat_alloc_output_context2 error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		ret = AVERROR_UNKNOWN;
		goto end;
	}

	pOutAVFmt = pofmt_ctx->oformat;

	/* 为输出多媒体文件创建流并且拷贝流参数 */
	for (i = 0; i < pifmt_ctx->nb_streams; i++)
	{
		AVStream *pInStream = pifmt_ctx->streams[i];
		AVStream *pOutStream = avformat_new_stream(pofmt_ctx, NULL);;
		if (!pOutStream)
		{
			sprintf(temp_buf, "avformat_new_stream error!\n");
			AfxMessageBox(temp_buf);
			OutputDebugString(temp_buf);
			ret = AVERROR_UNKNOWN;
			goto end;
		}

		/* 拷贝参数 */
		ret = avcodec_parameters_copy(pOutStream->codecpar, pInStream->codecpar);
		if (ret < 0)
		{
			sprintf(temp_buf, "avcodec_parameters_copy error!\n");
			OutputDebugString(temp_buf);
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
		OutputDebugString(temp_buf);
		goto end;
	}
	isOpen = 1;

	/* 写多媒体文件头 */
	ret = avformat_write_header(pofmt_ctx, NULL);
	if (ret < 0)
	{
		sprintf(temp_buf, "avformat_write_header error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		goto end;
	}

// 
// 	/* 移动到相应的时间点 */
// 	ret = av_seek_frame(pifmt_ctx, -1, startTime*AV_TIME_BASE, AVSEEK_FLAG_ANY);
// 	if (ret < 0)
// 	{
// 		sprintf(temp_buf, "av_seek_frame error!\n");
// 		AfxMessageBox(temp_buf);
// 		OutputDebugString(temp_buf);
// 		goto end;
// 	}


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
		frame_cnt++;
		sprintf(temp_buf, "video 1 process frame %d", frame_cnt);
		/*dlg->SetStatusMessage(temp_buf);*/
		OutputDebugString(temp_buf);
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
		pkt.pts = av_rescale_q_rnd(pkt.pts /*- ptsStartTime[pkt.stream_index]*/, pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(pkt.dts /*- dtsStartTime[pkt.stream_index]*/, pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		if (pkt.pts < 0)
			pkt.pts = 0;

		if (pkt.dts < 0)
			pkt.dts = 0;

		pkt.duration = (int)av_rescale_q((int64_t)pkt.duration, pInStream->time_base, pOutStream->time_base);
		pkt.pos = -1;

		last_dts = pkt.dts;
		last_pts = pkt.pts;
		/* 写数据 */
		ret = av_interleaved_write_frame(pofmt_ctx, &pkt);
		if (ret < 0)
		{
			sprintf(temp_buf, "av_interleaved_write_frame error!\n");
			AfxMessageBox(temp_buf);
			OutputDebugString(temp_buf);
			break;
		}

		av_packet_unref(&pkt);
	}

	//--------------------------
	/* 打开输入多媒体文件 */
	if ((ret = avformat_open_input(&pifmt_ctx, pSrc1, 0, 0)) < 0)
	{
		sprintf(temp_buf, "avformat_open_input error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		goto end;
	}
	frame_cnt_total = frame_cnt;
	frame_cnt = 0;
	while (1)
	{
		AVStream *pInStream, *pOutStream;

		ret = av_read_frame(pifmt_ctx, &pkt);
		if (ret < 0)
			break;
		frame_cnt++;
		sprintf(temp_buf, "video 2 process frame %d, %d", frame_cnt, frame_cnt_total);
		/*dlg->SetStatusMessage(temp_buf);*/
		OutputDebugString(temp_buf);
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
		pkt.pts = av_rescale_q_rnd(last_dts + pkt.pts /*- ptsStartTime[pkt.stream_index]*/, pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(last_pts + pkt.dts /*- dtsStartTime[pkt.stream_index]*/, pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		if (pkt.pts < 0)
			pkt.pts = 0;

		if (pkt.dts < 0)
			pkt.dts = 0;

		pkt.duration = (int)av_rescale_q((int64_t)pkt.duration, pInStream->time_base, pOutStream->time_base);
		pkt.pos = -1;

		/* 写数据 */
		ret = av_interleaved_write_frame(pofmt_ctx, &pkt);
		if (ret < 0)
		{
			sprintf(temp_buf, "av_interleaved_write_frame error!\n");
			AfxMessageBox(temp_buf);
			OutputDebugString(temp_buf);
			break;
		}

		av_packet_unref(&pkt);
	}
	//--------------------------
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


void CConcatDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

}
