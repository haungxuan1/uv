#include "pch.h"
#include <WinSock2.h>
#include "PrtTCP.h"
#include "Log.h"
#include "PQMsg.h"
#pragma comment(lib, "WS2_32.lib")

CPrtTCP::CPrtTCP():
	m_bThread(false), m_bTermination(false), m_lpHandle(0)
{
	m_vThParam = make_tuple(
		__FUNCTION__,
		this, 
		&m_lLock, 
		&m_cSend, 
		&m_cTermination, 
		&m_bTermination, 
		&m_vParams, 
		bind(&CPrtTCP::OnTCP, this, placeholders::_1));
}

CPrtTCP::~CPrtTCP()
{

}

bool CPrtTCP::Reg()
{
	_cwprintf(L"%s\r\n", __FUNCTIONW__);

	Restart(m_bTermination, m_lLock);

	HANDLE lpHandle = (HANDLE)_beginthreadex(NULL, 0, OnWorkThread, (void*)&m_vThParam, 0, NULL);

	CloseHandle(lpHandle);

	if (!(m_bThread = (!!lpHandle))) { cout << "_beginthreadex"  << endl; return false; }

	WSADATA ws;

	if (!!WSAStartup(MAKEWORD(2, 2), &ws)) { cout << "WSAStartup" << endl; ; return false; }

	m_lpHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_lpHandle == INVALID_SOCKET) { cout << "socket handle" << endl;; return false; }

	struct sockaddr_in tServerAddr;

	tServerAddr.sin_family = AF_INET;

	tServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	tServerAddr.sin_port = htons(9100);

	memset(tServerAddr.sin_zero, 0, sizeof(tServerAddr.sin_zero));

	bool bConnect = (connect(m_lpHandle, (struct sockaddr*)&tServerAddr, sizeof(tServerAddr)) != SOCKET_ERROR);

	cout << "c: " << bConnect << endl;

	return bConnect;
}

void CPrtTCP::Unreg()
{
	m_bThread ? CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnExitThread, (void*)&m_vThParam, 0, NULL)) : []()->BOOL {return 0; }();

	m_bThread ? m_cTermination.wait() : []()->bool { return 0; }(); m_bThread = false;

	m_lpHandle ? closesocket(m_lpHandle): []()->int { return 0; }(); m_lpHandle = 0;

	WSACleanup();

	_cwprintf(L"%s\r\n", __FUNCTIONW__);
}

void CPrtTCP::AllocateMmemory()
{

}

void CPrtTCP::DeallocateMemory()
{
	for (auto& iParam : m_vParams) 
	{ 
		array<unsigned int, 3>* lpUserParam = (array<unsigned int, 3>*)get<ePI_UserParam>(iParam);
		
		delete lpUserParam; 
	}
	m_vParams.clear();
}

void CPrtTCP::TCP(vParam iParam)
{
	Pushback(iParam, m_vParams, m_lLock);

	m_cSend.signal();
}

void CPrtTCP::OnTCP(vParam& iParam)
{
	auto& iFrameId = get<ePI_FrameId>(iParam);

	auto& iImPtr = get<ePI_ImPtr>(iParam);

	array<unsigned int,3>*lpUserParam = (array<unsigned int, 3>*)get<ePI_UserParam>(iParam);

	auto& iFinish = (*lpUserParam)[0], &iSent = (*lpUserParam)[1], &iSize = (*lpUserParam)[2];

	bool bPkgSent = OnSend(iSent, iSize, iImPtr.Get()->data);

	if (bPkgSent && iFinish) ::PostMessage(m_lpInitiator->GetSafeHwnd(), PQM_MATCH_IM_TCP_FINISH, 0, 0);

	bPkgSent ? [&lpUserParam]() { delete lpUserParam; }() : Pushfront(iParam, m_vParams, m_lLock);
}

bool CPrtTCP::OnSend(unsigned int& uSent, unsigned int& uSize, unsigned char*& lpBuffer)
{
	timeval tTimer;

	tTimer.tv_sec = 1; tTimer.tv_usec = 0;

	fd_set tWrite;

	FD_ZERO(&tWrite);

	FD_SET(m_lpHandle, &tWrite);

	int nRes = select(0, nullptr, &tWrite, nullptr, &tTimer);

	if (!!!nRes) return false;

	if (nRes < 0) return true;

	unsigned int uResidual = uSize - uSent, uPkgSize = 1024 * 100;

	unsigned int uCurSend = (uPkgSize < uResidual ? uPkgSize : uResidual);

	int nSent = send(m_lpHandle, (char*)(lpBuffer + uSent), uCurSend, 0);

	if (nSent <= 0) return true;

	uSent += nSent;

	return uSize <= uSent;
}