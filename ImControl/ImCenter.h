#pragma once
#include "ImThCenter.h"
#include "PQMsg.h"

namespace ImRender { class CImRender; }
namespace ImControl2 { class CImLayerMsg; class CImParamMsg; class CImViewMsg; class CImControl; }

namespace ImCenter
{
	class CImCenter : public ImThCenter::CImThCenter
	{
		friend class ImRender::CImRender;
		friend class ImControl2::CImLayerMsg;
		friend class ImControl2::CImParamMsg;
		friend class ImControl2::CImViewMsg;
		friend class ImControl2::CImControl;

	private:
		CImCenter();
		virtual ~CImCenter();

	private:
		virtual void PrSize(double& w, double& h) { w = m_vPreviewSize[0], h = m_vPreviewSize[1]; }
		virtual void Render(long long& nSign) { ::PostMessage(m_lpHandle->GetSafeHwnd(), PQM_IM_CENTER_SYNC, 0, nSign); }

	protected:
		void Bind(CWnd* lpHandle) { m_lpHandle = lpHandle; }
		void PreviewSize(int w, int h) { m_vPreviewSize = { w, h }; }

	protected:
		void CreateLayer(ImLayer::tagLayer& tInfo);
		void DestroyLayer(long long nSign);
		void ClearLayer();

	protected:
		void SyncLayerType(ImLayer::tagLayer& tInfo);

	protected:
		long long GetFocusLayer() { return m_nFocus; }
		long long GetBackground() { return m_nBackground; }

	public:
		bool GetSyncInfo(long long nSign, ImLayer::tagLayer& tInfo); // 外部获取信息同步接口
		bool GetRenderParam(long long nSign, ImLayer::tagLayer& tInfo);// 渲染线程同步接口

	public:
		void Background(long long nBackground);
		void Focus(long long nFocus);
		void Enable(long long nSign, bool bEnable);
		void Name(long long nSign, CString rName);
		void Left(long long nSign, int nLeft);
		void Top(long long nSign, int nTop);
		void Width(long long nSign, int nWidth);
		void Height(long long nSign, int nHeight);
		void Opacity(long long nSign, double dOpacity);
		void HorScal(long long nSign, double dScal);
		void VerScal(long long nSign, double dScal);
		void Angle(long long nSign, double dAngle);
		void Associate(long long nSign, bool bAssociate);
		void Moving(long long nSign);
		void Scaling(long long nSign);
		void Rotating(long long nSign);
		void Selecting(long long nSign);
		void Moving(long long nSign, int x, int y);
		void Move(long long nSign, int x, int y);
		void Scaling(long long nSign, int x, int y);
		void Scale(long long nSign, int x, int y);
		void Scale(long long nSign, double dScale);
		void Scale(double dScale);
		void Rotating(long long nSign, double dAngle);
		void Rotate(long long nSign, double dAngle);
		void Contrast(double dCon);

	protected:	//	对外接口
		void GetSign(vector<long long>& vSign);
		bool Get(long long nSign, ImLayer::tagLayer& tParam); // 全取值 - 慎用
		void Set(long long nSign, ImLayer::tagLayer& tParam); // 全赋值 - 慎用

	private:
		void Sign(vector<long long>& vSign);
		void Clear();
		void Push(ImLayer::tagLayer& tParam);
		void Pop(long long nSign);
		
	private:
		long long m_nFocus; // 当前焦距图层
		long long m_nBackground; // 背景图层

	private:
		CWnd* m_lpHandle;
		array<int, 2> m_vPreviewSize;

	};
}
