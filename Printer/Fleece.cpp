#include "pch.h"
#include "Fleece.h"
#include <WinSock2.h>
#include "PQMsg.h"

CFleece::CFleece():
    m_lpHandle(0), m_uThread(0), m_uWorking(0), m_uTermination(0x11), m_bFleece(true)
{

}

CFleece::~CFleece()
{

}

bool CFleece::Reg()
{
    if (!CImRotate::Reg()) return false;

    if (m_uPrinterType != ePT_FastPrint) { SetFleece(false);}//  return true; }

   

    _cwprintf(L"%s\r\n", __FUNCTIONW__);

    m_lpHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_lpHandle == INVALID_SOCKET) return false;

    struct sockaddr_in tServerAddr;

    tServerAddr.sin_family = AF_INET;

    tServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    tServerAddr.sin_port = htons(6668);

    memset(tServerAddr.sin_zero, 0, sizeof(tServerAddr.sin_zero));

    if (connect(m_lpHandle, (struct sockaddr*)&tServerAddr, sizeof(tServerAddr)) == SOCKET_ERROR) return false;

    HANDLE lpRecv = (HANDLE)_beginthreadex(NULL, 0, OnRecvThread, (void*)this, 0, NULL);

    HANDLE lpSend = (HANDLE)_beginthreadex(NULL, 0, OnSendThread, (void*)this, 0, NULL);

    HANDLE lpPrt = (HANDLE)_beginthreadex(NULL, 0, OnPrtThread, (void*)this, 0, NULL);

    CloseHandle(lpRecv); CloseHandle(lpSend); CloseHandle(lpPrt);

    uchar uThStatus = ( ((uchar)(!!lpRecv) << 0x2) | ((uchar)(!!lpSend) << 0x1) | ((uchar)(!!lpPrt)));

    SetThStatus(uThStatus); SetWkStatus(uThStatus); SetTmStatus(uThStatus);

    return (uThStatus == 0x07);
}

void CFleece::Unreg()
{
    if (m_uPrinterType != ePT_FastPrint) { CImRotate::Unreg(); return; }


    _cwprintf(L"%s 1\r\n", __FUNCTIONW__);

    CloseHandle((HANDLE)_beginthreadex(NULL, 0, OnTermThread, (void*)this, 0, NULL));

//    m_cTermination.wait();

    _cwprintf(L"%s 2\r\n", __FUNCTIONW__);

    ClearThStatus(0x3);

    _cwprintf(L"%s\r\n", __FUNCTIONW__);

    CImRotate::Unreg();

    _cwprintf(L"%s\r\n", __FUNCTIONW__);

    m_vParams.clear();
}

void CFleece::AllocateMmemory()
{
    CImRotate::AllocateMmemory();

}

void CFleece::DeallocateMemory()
{

    CImRotate::DeallocateMemory();
}

void CFleece::SetThStatus(uchar uTh)
{
    CSWLock lock(&m_lLock);

    m_uThread |= uTh;
}

bool CFleece::GetThStatus(uchar uTh)
{
    CSRLock lock(&m_lLock);

    return !!(m_uThread & uTh);
}

void CFleece::ClearThStatus(uchar uTh)
{
    CSWLock lock(&m_lLock);

    uTh = (~uTh);

    m_uThread &= uTh;
}

void CFleece::SetWkStatus(uchar uTh)
{
    CSWLock lock(&m_lLock);

    m_uWorking |= uTh;
}

bool CFleece::GetWkStatus(uchar uTh)
{
    CSRLock lock(&m_lLock);

    return !!(m_uWorking & uTh);
}

void CFleece::ClearWkStatus(uchar uTh)
{
    CSWLock lock(&m_lLock);

    uTh = (~uTh);

    m_uWorking &= uTh;
}

void CFleece::SetTmStatus(uchar uTh)
{
    CSWLock lock(&m_lLock);

    m_uTermination |= uTh;
}

bool CFleece::ClearTmStatus(uchar uTh)
{
    CSWLock lock(&m_lLock);

    uTh = (~uTh);

    m_uTermination &= uTh;

    return !!!m_uTermination;
}

unsigned int __stdcall CFleece::OnTermThread(void* lpUesrParam)
{
    CFleece* lpThis = (CFleece*)lpUesrParam;

    if (lpThis->GetThStatus(0x4)) lpThis->ClearWkStatus(0x4);

    if (lpThis->GetThStatus(0x2)) { lpThis->ClearWkStatus(0x2); lpThis->m_cSend.signal(); }

    if (lpThis->GetThStatus(0x1)) { lpThis->ClearWkStatus(0x1); lpThis->m_cPrt.signal(); }

    return 0;
}

unsigned int __stdcall CFleece::OnRecvThread(void* lpUesrParam)
{
    CFleece* lpThis = (CFleece*)lpUesrParam;

    timeval tTimer;

    tTimer.tv_sec = 1; tTimer.tv_usec = 0;

    fd_set tRead;

    unsigned char szCmd[276] = { 0 }; // 服务端一问一答，暂时不用考虑粘包问题 // 276 = 改命令格式下所能达到最大长度

    while (lpThis->GetWkStatus(0x4))
    {
        FD_ZERO(&tRead);

        FD_SET(lpThis->m_lpHandle, &tRead);

        int nRes = select(0, &tRead, nullptr, nullptr, &tTimer);

        if (nRes < 0)
        {
            cout << "recv link" << endl;
            //PostMessage PQM_MATCH_FLEECE_RECV
            continue;// tcp 连接出错        
        }
        else if (!!!nRes) { continue; } // 超时

        int nRecv = recv(lpThis->m_lpHandle, (char*)szCmd, sizeof(szCmd), 0);

        if (nRecv <= 0) continue;

        unsigned char* lpPkg = new unsigned char[nRecv]; // 包丢出

        std::memcpy(lpPkg, szCmd, nRecv); Sleep(100); // 用于减缓通讯压力

        ::PostMessage(lpThis->m_lpInitiator->GetSafeHwnd(),
            PQM_FLEECE_RECV, (WPARAM)nRecv, (LPARAM)lpPkg);
    }
    cout << __FUNCTION__ << endl;

    if (lpThis->ClearTmStatus(0x4)) lpThis->m_cTermination.signal();

    return 0;
}

unsigned int __stdcall CFleece::OnSendThread(void* lpUesrParam)
{
    CFleece* lpThis = (CFleece*)lpUesrParam;

    timeval tTimer;

    tTimer.tv_sec = 1; tTimer.tv_usec = 0;

    fd_set tWrite;

    tuple<unsigned char*, int, int, bool> vCmd;

    while (lpThis->GetWkStatus(0x2))
    {
        // 服务端一问一答，暂时不用考虑粘包问题
        if (!lpThis->Pop(vCmd)) { lpThis->m_cSend.wait(); continue; }

        FD_ZERO(&tWrite);

        FD_SET(lpThis->m_lpHandle, &tWrite);

        int nRes = select(0, nullptr, &tWrite, nullptr, &tTimer);

        if (nRes < 0)
        {
            cout << "send link error" << endl;
            //PostMessage
            continue;// tcp 连接出错        
        }

        if (!!!nRes) { lpThis->Insert(vCmd); continue; } // 超时则重新压入

        int nSent = send(lpThis->m_lpHandle, (char*)get<0>(vCmd), get<1>(vCmd), 0);

        if (nSent <= 0)
        {
            cout << "send link error2" << endl;
            //PostMessage
            continue;// tcp 连接出错    
        }
        get<3>(vCmd) ? [&vCmd]() { delete[] get<0>(vCmd); }() : []() {}();
    }
    cout << __FUNCTION__ << endl;

    if (lpThis->ClearTmStatus(0x2)) lpThis->m_cTermination.signal();

    return 0;
}

unsigned int __stdcall CFleece::OnPrtThread(void* lpUesrParam)
{
    CFleece* lpThis = (CFleece*)lpUesrParam;

    while (lpThis->GetWkStatus(0x1))
    {
        if (lpThis->IsFleece()) { Sleep(100); continue; } // 这里采取sleep代替信号

        vParam iParam; // 这里必须包含智能指针

        if (!lpThis->Pop(iParam)) { lpThis->m_cPrt.wait(); continue; }

        lpThis->CPrtTCP::TCP(iParam);
    }
    cout << __FUNCTION__ << endl;

    if (lpThis->ClearTmStatus(0x1)) lpThis->m_cTermination.signal();

    return 0;
}

void CFleece::OnFleece(vParam& iParam)
{
    Push(iParam);

    m_cPrt.signal();
}

void CFleece::Push(vParam& iParam)
{
    CSWLock lock(&m_lLock);

    m_vParams.push_back(iParam);
}

bool CFleece::Pop(vParam& iParam)
{
    bool bEmpty = true;
    {
        CSWLock lock(&m_lLock);

        if (!(bEmpty = m_vParams.empty()))
        {
            auto iPkg = m_vParams.begin();

            iParam = (*iPkg);

            iPkg = m_vParams.erase(iPkg);
        }
    }
    cout << __FUNCTION__ << ": " << bEmpty << endl;

    return !bEmpty;
}

void CFleece::SetFleeceParam(vector<int> vOy)
{
    m_vOy = vOy;
}

void CFleece::FleeceParam(int& nOy)
{
    auto iOy = m_vOy.begin();

    nOy = (m_vOy.empty() ? (532.2f * GetSkipLine()) : (*iOy));

    iOy = (m_vOy.empty() ? iOy : m_vOy.erase(iOy));
}

void CFleece::Begin()
{
    SetFleece(true);
}

void CFleece::End()
{
    SetFleece(false);
}

bool CFleece::IsFleece()
{
    CSRLock lock(&m_lLock);

    return m_bFleece;
}

void CFleece::SetFleece(bool bFleece)
{
    CSWLock lock(&m_lLock);

    m_bFleece = bFleece;
}

void CFleece::PushCmd(tuple<unsigned char*, int, int, bool> vCmd)
{
    Push(vCmd);

    m_cSend.signal();
}

void CFleece::Push(tuple<unsigned char*, int, int, bool> vCmd)
{
    CSWLock lock(&m_lLock);

    m_vCmdPkg.push_back(vCmd);
}

void CFleece::Insert(tuple<unsigned char*, int, int, bool> vCmd)
{
    CSWLock lock(&m_lLock);

    m_vCmdPkg.insert(m_vCmdPkg.begin(), vCmd);
}

bool CFleece::Pop(tuple<unsigned char*, int, int, bool>& vCmd)
{
    bool bEmpty = true;
    {
        CSWLock lock(&m_lLock);

        if (!(bEmpty = m_vCmdPkg.empty()))
        {
            auto iCmd = m_vCmdPkg.begin();

            vCmd = (*iCmd);

            iCmd = m_vCmdPkg.erase(iCmd);
        }
    }
    return !bEmpty;
}

unsigned char CFleece::Sum(unsigned char* lpParam, int nSize)
{
    unsigned char uSum = 0;

    for (int i = 0; i < nSize; ++i) uSum += lpParam[i];

    return uSum;
}

void CFleece::CMD(unsigned char uPriCMD, unsigned char uSecCMD, wstring strParam, unsigned char*& lpCmd, int& nCmdSize)
{
    int nParamSize = strParam.size() * sizeof(wchar_t);

    nCmdSize = 24 + nParamSize; // 24 = begin + end + cmdsize + sum + PriCMD + SecCMD

    lpCmd = new unsigned char[nCmdSize];

    unsigned char szBegin[] = { 66,0,89,0,72,0,88,0,48,0,49,0 };

    unsigned char szEnd[] = { 66,0,89,0,72,0,88,0 };

    std::memcpy(lpCmd, szBegin, 12); // 12 = sizeof(szBegin)

    std::memcpy(lpCmd + nCmdSize - 8, szEnd, 8); // 8 = sizeof(szEnd)

    lpCmd[12] = nParamSize + 3; // 12 = sizeof(szBegin)

    lpCmd[13] = uPriCMD; 
    
    lpCmd[14] = uSecCMD;

    std::memcpy(lpCmd + 15, strParam.c_str(), nParamSize);

    lpCmd[15 + nParamSize] = Sum(lpCmd + 12, lpCmd[12]);
}

void CFleece::StateCmd(unsigned char*& lpCmd, int& nCmdSize)
{
    CMD(0x01, 0x02, L"1", lpCmd, nCmdSize);
}

void CFleece::InInPass(unsigned char*& lpCmd, int& nCmdSize)
{
    CMD(0x01, 0x21, L"1", lpCmd, nCmdSize);
}

void CFleece::Pos(unsigned char*& lpCmd, int& nCmdSize)
{
    CMD(0x01, 0x01, L"1", lpCmd, nCmdSize);
}

void CFleece::Stop(unsigned char*& lpCmd, int& nCmdSize)
{
    CMD(0x02, 0x05, L"1", lpCmd, nCmdSize);
}

void CFleece::X0(unsigned char*& lpCmd, int& nCmdSize)
{
    CMD(0x02, 0x08, L"1", lpCmd, nCmdSize);
}

void CFleece::Y0(unsigned char*& lpCmd, int& nCmdSize)
{
    CMD(0x02, 0x09, L"1", lpCmd, nCmdSize);
}

void CFleece::Move(int nDirection, int nSpeed, unsigned long long nDistance, unsigned char*& lpCmd, int& nCmdSize)
{
    std::wstring strParam;

    strParam.append(L"(").
        append(std::to_wstring(nDistance)).append(L",").
        append(std::to_wstring(nDirection)).append(L",").
        append(std::to_wstring(nSpeed)).append(L")");

    CMD(0x02, 0x10, strParam, lpCmd, nCmdSize);
}
