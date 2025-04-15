#pragma once
#define STDSKINSAPII    __declspec(dllimport) int __stdcall
#define STDSKINSAPID    __declspec(dllimport) DWORD __stdcall
#define STDSKINSAPIB    __declspec(dllimport) BOOL __stdcall

#ifndef EXTERN_C_START
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END }
#endif

EXTERN_C_START
//===========================================================

//��InitInstance()�������ڵ��ô˺�������Ƥ�����г�ʼ������������dlg.DoModal();���ڴ�֮ǰ����Ч����á�
STDSKINSAPII JD_Load();

//������ͣ��Ƥ��Ч�� Ĭ�Ͽ����������
//BShowSkin TRUE����Ƥ��Ч�� FALSE�ر�Ƥ��Ч��
STDSKINSAPII JD_Skin(BOOL BSkinShow = TRUE);

//������ر�ָ���ؼ���Ƥ��Ч�� ���һ���ؼ��������Ի����� OWNERDRAW ��������ɹ�
//hCtrWnd ����Ҫ������رյĴ��ھ��
//BShowSkin TRUE����Ƥ��Ч�� FALSE�ر�Ƥ����ʾ
STDSKINSAPII JD_CtrlSkin(HWND hCtrWnd, BOOL BSkinShow = TRUE);

//��ȡ������ʹ�õı���ɫ
STDSKINSAPID JD_GetBgColor();

//�����Զ�������Ϊ��MDI���һЩ������Ϊϵͳά���ķָ����޷����ƶ��趨�� ����ͬʱ��Ӷ�� �� | �������ɣ�ֻ��DIALOG��Ч��
#define CUSTOM_WS_ETCHEDLEFT        0x2             //�ڴ��ڿͻ�����߻��Ʒָ�����ע�⣺��Ӵ���ʽ��ζ��Ƥ����ӹ�WM_PAINT��Ϣ���û����Զ�����ƽ�ʧЧ
#define CUSTOM_WS_ETCHEDRIGHT       0x4             //�ڴ��ڿͻ����ұ߻��Ʒָ�����ע�⣺��Ӵ���ʽ��ζ��Ƥ����ӹ�WM_PAINT��Ϣ���û����Զ�����ƽ�ʧЧ
#define CUSTOM_WS_ETCHEDTOP         0x8             //�ڴ��ڿͻ����������Ʒָ�����ע�⣺��Ӵ���ʽ��ζ��Ƥ����ӹ�WM_PAINT��Ϣ���û����Զ�����ƽ�ʧЧ
#define CUSTOM_WS_ETCHEDBOTTOM      0x10            //�ڴ��ڿͻ����ײ����Ʒָ�����ע�⣺��Ӵ���ʽ��ζ��Ƥ����ӹ�WM_PAINT��Ϣ���û����Զ�����ƽ�ʧЧ

//�޸��ڲ��Զ�����ʽ(����ʽ�ʹ�����ʽ�޹أ���Ƥ���ڲ��Զ�ʹ�õ�)
//dwRemove ��Ҫɾ������ʽ
//dwAdd ��Ҫ��ӵ���ʽ
//�ɹ�����TRUE ʧ�ܷ���FALSE (ע�⣺���dwRemove��dwAddͬʱ��ֵʱִ��˳�����ȼ����)
STDSKINSAPID JD_ModifyStyle(DWORD dwRemove, DWORD dwAdd, HWND hDialog);


#define WND_TYPE_DIADLG            1              //��������
#define WND_TYPE_BUTTON            2              //��ť
#define WND_TYPE_CHECKBOX          3              //��ѡ��ť
#define WND_TYPE_RADIOBTN          4              //��ѡ��ť
#define WND_TYPE_EDIT              5              //�ı��༭��
#define WND_TYPE_COMBOBOX          6              //��Ͽ�
#define WND_TYPE_COMBOLBOX         7              //ComboLBox����б��
#define WND_TYPE_GROUPBOX          8              //GroupBox��ѡ��ť
#define WND_TYPE_STATIC            9              //Static��̬�ı�
#define WND_TYPE_SCROLLBAR        10              //ScrollBar�������ؼ�
#define WND_TYPE_LISTBOX          11              //ListBox�б��
#define WND_TYPE_LISTVIEW         12              //ListView�б���ͼ
#define WND_TYPE_HEADER           13              //HeaderControl�б�ͷ
#define WND_TYPE_HOT_KEY          14              //HotKey�ȼ�
#define WND_TYPE_TRACK_BAR        15              //TrackBar����
#define WND_TYPE_UPDOWN           16              //UpDown�л���ť
#define WND_TYPE_PROGRESS         17              //Progress������
#define WND_TYPE_TREEVIEW         18              //TreeView����ͼ�ؼ�
#define WND_TYPE_TAB              19              //Tab���
#define WND_TYPE_IPADDR           20              //IPAddress�ؼ�
#define WND_TYPE_SYSLINK          21              //SysLink�����ӿؼ�
#define WND_TYPE_STATUSBAR        22              //StatusBar״̬���ؼ�
#define WND_TYPE_TOOLBAR          23              //ToolBar�������ؼ�
#define WND_TYPE_REBAR            24              //ReBar���й���ؼ�
#define WND_TYPE_ERASE            25              //BGֻ���Ʊ���ɢ��
#define WND_TYPE_MENUS            26              //Menus�˵���
#define WND_TYPE_MDIDIADLG        27              //MDI��ܴ���
#define WND_TYPE_MDICLIENT        28              //MDIClient��ܿͻ�����
#define WND_TYPE_MDICHILD         29              //MDIClient�Ӵ���
#define WND_TYPE_AFXCONTROLBAR    30              //AfxControlBarͣ������
#define WND_TYPE_AFXFRAMEORVIEW   31              //AfxFrameOrView
#define WND_TYPE_AFXMDIFRAME      32              //AfxMDIFrame�и�������

//ĳЩ����ϵͳ�����Զ�ʶ��ʱ�����ֶ���ӽ�ȥ
//hWnd ��Ҫ��ӵĴ��ھ��
//BWndType �������� �ο����Ϻ궨��
//�ɹ�����0 ��������ӷ���1 ���Ͳ����ڷ���2
STDSKINSAPID JD_AddWnd(HWND hWnd, BYTE BWndType);

//��ָ�������Ƴ�Ƥ���ӹ��б� Ƥ�������ٸ�Ԥ�����κ���Ϊ ���װ���
//�ɹ�����TRUE ʧ�ܷ���FALSE
STDSKINSAPID JD_RemoveWnd(HWND hWnd);

//����ָ�����������С �ɹ�����TRUE ʧ�ܷ���FALSE
//BWndType �������� �ο����Ϻ궨��
//nFontSize ��Ҫ���õ������С
STDSKINSAPIB JD_SetFontSize(BYTE BWndType, DWORD dwFontSize);
//===========================================================
EXTERN_C_END
