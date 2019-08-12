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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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
	// 按键在滑动块区域内，不做操作
	CRect tRect;
	GetThumbRect(&tRect);
	if (tRect.PtInRect(point))
	{
		CSliderCtrl::OnLButtonDown(nFlags, point);
		return;
	}
	// 实现滑动块精确定位   
	CRect rcSlider;
	GetChannelRect(&rcSlider);
	int nMax = GetRangeMax();
	float fInterval = float(rcSlider.Width()) / nMax;
	float fPos = (point.x - rcSlider.left + fInterval - 1) / fInterval;

	CSliderCtrl::SetPos((int)fPos);
}
