// ConcatDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfc_ffmpeg_streamer.h"
#include "ConcatDialog.h"
#include "afxdialogex.h"
#include <list>
using namespace std;

// CConcatDialog �Ի���

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


// CConcatDialog ��Ϣ�������


BOOL CConcatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
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
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CConcatDlg::OnBnClickedBtnConvd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	if (nCol == 1 && nRow != -1) //���ѡ�������������Ϊ��;
	{
		m_edit1.ShowWindow(SW_HIDE);
		int i = 0;
		BOOL isOpen = TRUE;		//�Ƿ��(����Ϊ����)
		CString defaultDir = ".\\";	//Ĭ�ϴ򿪵��ļ�·��
		CString fileName = "";			//Ĭ�ϴ򿪵��ļ���
		CString filter = "�ļ� (*.*)|*.*||";	//�ļ����ǵ�����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
	CRect rc;
	int nRow = pNMListView->iItem;
	int nCol = pNMListView->iSubItem;

	//CString str;
	//str.Format("%d, %d", nRow, nCol);
	//AfxMessageBox(str);
	if (nCol == 0 ) //���ѡ����Ƿ�����;
	{
		
		m_mflist.GetSubItemRect(nRow, nCol, LVIR_LABEL, rc);//��������RECT��
		m_edit1.SetParent(&m_mflist);//ת������Ϊ�б���е�����
		m_edit1.MoveWindow(rc);//�ƶ�Edit��RECT���ڵ�λ��;
		m_edit1.SetWindowText(m_mflist.GetItemText(nRow, nCol));//���������е�ֵ����Edit�ؼ��У�
		m_edit1.ShowWindow(SW_SHOW);//��ʾEdit�ؼ���
		m_edit1.SetFocus();//����Edit����
		//m_edit1.ShowCaret();//��ʾ���
		m_edit1.SetSel(0,-1);//������ƶ������
		m_edit1.ShowWindow(SW_SHOW);

	}

	*pResult = 0;
}


void CConcatDlg::OnClickedAddlvim()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		int icnt;
		icnt = m_mflist.GetItemCount();
		m_mflist.InsertItem(icnt, "");
		m_mflist.SetItemText(icnt, 1, "...");
}


void CConcatDlg::OnRclickListfile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMenu menu;
	menu.LoadMenu(IDR_MENU2);      //���ز˵���Դ
	CMenu* pPopup = menu.GetSubMenu(0);   //��ôμ��˵�
	CPoint myPoint;
	GetCursorPos(&myPoint);      //��ȡ�������							
	pPopup->TrackPopupMenu(TPM_LEFTALIGN, myPoint.x, myPoint.y, this); //�����˵�

	*pResult = 0;
}


void CConcatDlg::OnMoveDown() //���ƶ� �˵�
{
	// TODO: �ڴ���������������

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
	/* �������ý���ļ� */
	if ((ret = avformat_open_input(&pifmt_ctx, pSrc, 0, 0)) < 0)
	{
		sprintf(temp_buf, "avformat_open_input error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		goto end;
	}

	/* ��ӡ�����ý���ļ�����Ϣ */
	av_dump_format(pifmt_ctx, 0, pSrc, 0);
#endif 
	//pifmt_ctx = in_cvinfo.pifmt_ctx;
	/* ������ļ� */
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

	/* Ϊ�����ý���ļ����������ҿ��������� */
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

		/* �������� */
		ret = avcodec_parameters_copy(pOutStream->codecpar, pInStream->codecpar);
		if (ret < 0)
		{
			sprintf(temp_buf, "avcodec_parameters_copy error!\n");
			OutputDebugString(temp_buf);
		}

		pOutStream->codecpar->codec_tag = 0;

	}

	av_dump_format(pofmt_ctx, 0, pDst, 1);

	/* �������ý���ļ���׼��д���� */
	ret = avio_open(&pofmt_ctx->pb, pDst, AVIO_FLAG_WRITE);
	if (ret < 0)
	{
		sprintf(temp_buf, "avio_open error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		goto end;
	}
	isOpen = 1;

	/* д��ý���ļ�ͷ */
	ret = avformat_write_header(pofmt_ctx, NULL);
	if (ret < 0)
	{
		sprintf(temp_buf, "avformat_write_header error!\n");
		AfxMessageBox(temp_buf);
		OutputDebugString(temp_buf);
		goto end;
	}

// 
// 	/* �ƶ�����Ӧ��ʱ��� */
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

		/* ת��ʱ��� */
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
		/* д���� */
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
	/* �������ý���ļ� */
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

		/* ת��ʱ��� */
		pkt.pts = av_rescale_q_rnd(last_dts + pkt.pts /*- ptsStartTime[pkt.stream_index]*/, pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		pkt.dts = av_rescale_q_rnd(last_pts + pkt.dts /*- dtsStartTime[pkt.stream_index]*/, pInStream->time_base, pOutStream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
		if (pkt.pts < 0)
			pkt.pts = 0;

		if (pkt.dts < 0)
			pkt.dts = 0;

		pkt.duration = (int)av_rescale_q((int64_t)pkt.duration, pInStream->time_base, pOutStream->time_base);
		pkt.pos = -1;

		/* д���� */
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

}
