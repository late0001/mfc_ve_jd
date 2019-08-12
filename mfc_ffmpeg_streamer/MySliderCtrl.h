#pragma once
#include "afxcmn.h"
class CMySliderCtrl :
	public CSliderCtrl
{
public:
	CMySliderCtrl();
	~CMySliderCtrl();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

