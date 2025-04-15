#include "pch.h"
#include "FleeceDlg.h"
#include "PQMsg.h"
#include "Fleece.h"

IMPLEMENT_DYNAMIC(CFleeceDlg, CCameraParamDlg)

BEGIN_MESSAGE_MAP(CFleeceDlg, CCameraParamDlg)
	ON_MESSAGE(PQM_FLEECE_BEGIN, OnFleeceBeginMsg)
	ON_MESSAGE(PQM_FLEECE_RESTRICT, OnFleeceRestrictMsg)
	ON_MESSAGE(PQM_FLEECE_SEND, OnFleeceSendMsg)
	ON_MESSAGE(PQM_FLEECE_SEND_STOP, OnFleeceSendStopMsg)
	ON_MESSAGE(PQM_FLEECE_RECV, OnFleeceRecvMsg)
	ON_MESSAGE(PQM_FLEECE_PRT, OnFleecePrtMsg)
	ON_MESSAGE(PQM_FLEECE_END, OnFleeceEndMsg)
END_MESSAGE_MAP()
CFleeceDlg::CFleeceDlg(CWnd* pParent) :
	CCameraParamDlg(pParent)
{

}

CFleeceDlg::~CFleeceDlg()
{

}

void CFleeceDlg::DoDataExchange(CDataExchange* pDX)
{
	CCameraParamDlg::DoDataExchange(pDX);
}

BOOL CFleeceDlg::OnInitDialog()
{
	CCameraParamDlg::OnInitDialog();

	unsigned char* lpCmd = nullptr; int nCmdSize = 0;

	PQPrinter().StateCmd(lpCmd, nCmdSize);

	m_vReqStatus = { lpCmd, nCmdSize, 0, false };

	PQPrinter().Pos(lpCmd, nCmdSize);

	m_vReqPos = { lpCmd, nCmdSize, 0, false };;

	return TRUE;
}

// prt.height 转脉冲数
int CFleeceDlg::PrtHeight2Pulse()
{
	// y轴 
	static double dPulseInMM = 25400.0 / 25.4;

	// pixel - 打印dpi
	int nPrtHeight = PQPrinter().GetPrintHeader()[1];

	// 打印dpi
	double dPrDPI = PQPrinter().GetPrinteDpi()[1];

	// 像素/mm
	double dPixelInMM = dPrDPI / 25.4;

	return dPulseInMM * nPrtHeight / dPixelInMM;
}

// 相机与墨头间距
int CFleeceDlg::Offset2Pulse()
{
	static double dPulseInMM = 25400.0 / 25.4;

	// 扫描与打印偏移 - #校准所得y向补偿值 
	// pixel - 相机dpi
	int nPrOffset = PQPrinter().GetOffset()[1];

	// 相机dpi
	double dCamDPI = PQPrinter().GetCmeraDpi()[1];

	double dPixelInMM = dCamDPI / 25.4;

	return dPulseInMM * nPrOffset / dPixelInMM;
}

// 机械原点与打印原点间距
int CFleeceDlg::Origin2Pulse()
{
	static double dPulseInMM = 25400.0 / 25.4;

	// pixel - 相机dpi
	double dDistance = 25629;

	// 相机dpi
	double dCamDPI = PQPrinter().GetCmeraDpi()[1];

	double dPixelInMM = dCamDPI / 25.4;

	return dPulseInMM * dDistance / dPixelInMM;
}

int CFleeceDlg::Oy2Pulse()
{
	// 1000
	static double dPulseInMM = 25400.0 / 25.4;

	// mm
	double dOy = 150; 

	return dPulseInMM * dOy;
}

// prt.height 转脉冲数
int CFleeceDlg::PrtWidth2Pulse()
{
	// x 轴
	static double dPulseInMM = 5080.0 / 25.4;

	double dMMTmp = 2000;

	return dMMTmp * dPulseInMM;
}

// skip line 转脉冲数 - #打印模块为了减少匹配次数,采取多行拼接后进行一次匹配,这里y轴步进与其保持一致
int CFleeceDlg::Skip2Pulse()
{
	static double dPulseInMM = 25400.0 / 25.4;

	int nPixelInStride = 532.2f * PQPrinter().GetSkipLine();

	double dCamDPI = PQPrinter().GetCmeraDpi()[1];

	double dPixelInMM = dCamDPI / 25.4;

	return dPulseInMM * nPixelInStride / dPixelInMM;
}

// y轴补偿值(单位：脉冲) 转像素数(相机dpi)
int CFleeceDlg::Compensate2Pixel(int nCompensate)
{
	static double dPulseInMM = 25400.0 / 25.4;

	double dCamDPI = PQPrinter().GetCmeraDpi()[1];

	double dPixelInMM = dCamDPI / 25.4;

	return dPixelInMM * nCompensate / dPulseInMM;
}

void CFleeceDlg::FleeceParam()
{
	// 1. x 归原点
	// 2. y 归原点
	// 3. y 后移 
	//		prt.height 转脉冲数
	//		相机与墨头间距 
	//		机械原点与打印原点间距
	//		#相机与墨头间距在校准时已经计算得出 
	//		#vOsInPulse[1] + vPrtInPulse[1]
	// 4. x 右移
	// 5. x 归原点
	//		#跳过相机采集时首行误差
	// 6. 扫描
	//		a. x 右移
	//		b. y 前移
	//		c. x 归原点

	isStop = false;
	unsigned char* lpCmd = nullptr; int nCmdSize = 0;

	// 1. x 归原点
	PQPrinter().X0(lpCmd, nCmdSize);
	m_vFleece.push_back({ lpCmd, nCmdSize, 1, true });

	// 2. y 归原点
	PQPrinter().Y0(lpCmd, nCmdSize);
	m_vFleece.push_back({ lpCmd, nCmdSize, 2, true });

	// 3. y 后移		
	int nPrtHeight = PrtHeight2Pulse();
	int nPrOffset = Offset2Pulse();
	int nOrigin = Origin2Pulse();
	int nOy = Oy2Pulse();
	int nBack = nPrtHeight + nPrOffset + nOrigin + nOy;
	PQPrinter().Move(CFleece::eDI_Back, CFleece::eSI_332, nBack, lpCmd, nCmdSize);
	m_vFleece.push_back({ lpCmd, nCmdSize, 3, true });

	// 4. x 右移
	int nPrtWidth = PrtWidth2Pulse();
	PQPrinter().Move(CFleece::eDI_Right, CFleece::eSI_700, nPrtWidth, lpCmd, nCmdSize);
	m_vFleece.push_back({ lpCmd, nCmdSize, 4, true });

	// 5. x 归原点 - #以左移替换归原点 - #归原点速度较慢
//	PQPrinter().X0(lpCmd, nCmdSize);
	PQPrinter().Move(CFleece::eDI_Left, CFleece::eSI_700, nPrtWidth, lpCmd, nCmdSize);
	m_vFleece.push_back({ lpCmd, nCmdSize, 5, true });

	// 6. 扫描
	int nSkip = Skip2Pulse();
	int nSize = floor((double)nPrOffset / nSkip) + m_nExtSize; // 填充y轴补偿值所需步进次数 + 5

	m_nExtSize = 0;

	int nCompensate = Compensate2Pixel(nPrOffset - nSkip * nSize);  // 将 nPrOffset 拆分为 n*nSkip 与 nCompensate两部分
	vector<int> vCompensate(nSize, 532.2f * PQPrinter().GetSkipLine()); // 步进与 Skip2Pulse().nPixelInStride 一致    532.2f
	!!nCompensate ? vCompensate.push_back(nCompensate) : []() {}();

	PQPrinter().SetFleeceParam(vCompensate);

	cout << "Com: " << nSize << " | " << vCompensate.size() << " | " << vCompensate[vCompensate.size() - 1] << endl;

	for (int i = 0; i < vCompensate.size(); ++i)
	{
		// a. x 右移
		PQPrinter().Move(CFleece::eDI_Right, CFleece::eSI_700, nPrtWidth, lpCmd, nCmdSize);
		m_vFleece.push_back({ lpCmd, nCmdSize, 6 + nSize * 3 + 0, true });

		// b. y 前移
		PQPrinter().Move(CFleece::eDI_Front, CFleece::eSI_332, nSkip, lpCmd, nCmdSize);
		m_vFleece.push_back({ lpCmd, nCmdSize, 6 + nSize * 3 + 1, true });

		// c. x 归原点
		//PQPrinter().X0(lpCmd, nCmdSize);
		PQPrinter().Move(CFleece::eDI_Left, CFleece::eSI_700, nPrtWidth, lpCmd, nCmdSize);
		m_vFleece.push_back({ lpCmd, nCmdSize, 6 + nSize * 3 + 2, true });
	}
}

LRESULT CFleeceDlg::OnFleeceBeginMsg(WPARAM wparam, LPARAM lparam)
{
	FleeceParam();

	PQPrinter().CFleece::Begin();

	PostMessage(PQM_FLEECE_SEND);

	return 0;
}

LRESULT CFleeceDlg::OnFleeceSendMsg(WPARAM wparam, LPARAM lparam)
{
	PQPrinter().PushCmd(m_vFleece[0]);

	PQPrinter().PushCmd(m_vReqStatus);

	return 0;
}

LRESULT CFleeceDlg::OnFleeceSendStopMsg(WPARAM wparam, LPARAM lparam)
{
	isStop = true;
	PQPrinter().PushCmd(m_vStop);

	PQPrinter().PushCmd(m_vReqStatus);

	return 0;
}

LRESULT CFleeceDlg::OnFleeceRecvMsg(WPARAM wparam, LPARAM lparam)
{
	if (isStop) {
		PostMessage(PQM_FLEECE_SEND_STOP_END);
		return 0;

	}
	int nPkgSize = (int)wparam;

	unsigned char* lpPkg = (unsigned char*)lparam;

	wstring strValue; unsigned char uSecCMD = 0;

	if (!PkgCheck(lpPkg, nPkgSize, uSecCMD, strValue)) { AfxMessageBox(L"Check Pkg Error"); return 0; }

	if (uSecCMD != 0x02) return 0;// 设备状态

	_wtoi(strValue.c_str()) != 1 ? PQPrinter().PushCmd(m_vReqStatus) : Forward();// 1 = 就绪

	return 0;
}

void CFleeceDlg::Forward()
{
	Sleep(1000);

	auto iPkg = m_vFleece.begin();

	iPkg = m_vFleece.erase(iPkg);

	m_vFleece.empty() ? PostMessage(PQM_FLEECE_PRT) : PostMessage(PQM_FLEECE_SEND);
}

void CFleeceDlg::Stop()
{
	unsigned char* lpCmd = nullptr; int nCmdSize = 0;
	if (!m_vFleece.empty())
		m_vFleece.clear();
	PQPrinter().Stop(lpCmd, nCmdSize);
	m_vStop={ lpCmd, nCmdSize, 1, true };
//	PostMessage(PQM_FLEECE_SEND_STOP);
	OnFleeceSendStopMsg(0,0);
}

LRESULT CFleeceDlg::OnFleecePrtMsg(WPARAM wparam, LPARAM lparam)
{
	cout << __FUNCTION__ << endl;

	PQPrinter().OnPrt();

	return 0;
}

bool CFleeceDlg::PkgCheck(unsigned char* lpPkg, int nPkgSize, unsigned char& uType, wstring& strValue)
{
	array<unsigned char, 12> vBegin{ 66,0,89,0,72,0,88,0,48,0,49,0 };

	array<unsigned char, 8> vEnd{ 66,0,89,0,72,0,88,0 };

	array<unsigned char, 12> vPkgBegin;

	array<unsigned char, 8> vPkgEnd;

	memcpy(vPkgBegin.data(), lpPkg, vPkgBegin.size());

	memcpy(vPkgEnd.data(), lpPkg + nPkgSize - vPkgEnd.size(), vPkgEnd.size());

	unsigned char uParamSize = lpPkg[12];

	unsigned char uPriCMD = lpPkg[13];

	unsigned char uSecCMD = lpPkg[14];

	unsigned char uValueSize = (uParamSize - 3) / sizeof(wchar_t);

	wchar_t* lpValue = new wchar_t[uValueSize + 1];

	memset(lpValue, 0, (uValueSize + 1) * sizeof(wchar_t));

	memcpy(lpValue, lpPkg + 15, uValueSize * sizeof(wchar_t));

	unsigned char uPkgSum = lpPkg[15 + uValueSize * sizeof(wchar_t)];

	unsigned char uSum = PQPrinter().Sum(lpPkg + 12, uParamSize);

	strValue = lpValue; delete[] lpValue; delete[] lpPkg;

	uType = uSecCMD;

	return (vPkgBegin == vBegin && vPkgEnd == vEnd && uPkgSum == uSum && uPriCMD == 0xfe);
}

LRESULT CFleeceDlg::OnFleeceEndMsg(WPARAM wparam, LPARAM lparam)
{
	PQPrinter().CFleece::End();

	return 0;
}

LRESULT CFleeceDlg::OnFleeceRestrictMsg(WPARAM wparam, LPARAM lparam)
{
	CPrinterBase::vParam* lpParam = (CPrinterBase::vParam*)lparam;

	PQPrinter().OnFleece(*lpParam);

	delete lpParam;

	return 0;
}