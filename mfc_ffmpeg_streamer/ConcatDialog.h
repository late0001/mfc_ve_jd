#pragma once
#include "ff_common.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "list"
using namespace std;
struct ab {
	int a;
	int b;
	char name[15];
};
// CConcatDialog �Ի���

class CConcatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConcatDlg)

public:
	CConcatDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConcatDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONCAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	list<struct ab> la;
	CEdit m_edit;
	virtual BOOL OnInitDialog();
	CListCtrl m_mflist;
	afx_msg void OnBnClickedBtnConvd();
	afx_msg void OnClickListfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkListfile(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_edit1;

	afx_msg void OnClickedAddlvim();
	afx_msg void OnRclickListfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMoveDown();
	afx_msg void OnBnClickedBtnConcat();
	afx_msg void OnBnClickedButton1();
};
