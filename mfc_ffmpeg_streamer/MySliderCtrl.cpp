#include "stdafx.h"
#include "MySliderCtrl.h"


CMySliderCtrl::CMySliderCtrl()
{
}


CMySliderCtrl::~CMySliderCtrl()
{
}
BEGIN_MESSAGE_MAP(CMySliderCtrl, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CMySliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CSliderCtrl::OnLButtonDown(nFlags, point);
#if 0
	CRect   rectClient, rectChannel;
	GetClientRect(rectClient);
	GetChannelRect(rectChannel);
	int nMax = 0;
	int nMin = 0;
	GetRange(nMin, nMax);
	int nPos =
		(nMax - nMin)*(point.x - rectClient.left - rectChannel.left) / (rectChannel.right - rectChannel.left);
	SetPos(nPos);
#endif
	// �����ڻ����������ڣ���������
	CRect tRect;
	GetThumbRect(&tRect);
	if (tRect.PtInRect(point))
	{
		CSliderCtrl::OnLButtonDown(nFlags, point);
		return;
	}
	// ʵ�ֻ����龫ȷ��λ   
	CRect rcSlider;
	GetChannelRect(&rcSlider);
	int nMax = GetRangeMax();
	float fInterval = float(rcSlider.Width()) / nMax;
	float fPos = (point.x - rcSlider.left + fInterval - 1) / fInterval;

	CSliderCtrl::SetPos((int)fPos);
}
