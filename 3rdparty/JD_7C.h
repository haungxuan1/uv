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

//在InitInstance()函数体内调用此函数加载皮肤进行初始化，尽可能在dlg.DoModal();窗口打开之前调用效果最好。
STDSKINSAPII JD_Load();

//开启或停用皮肤效果 默认开启无须理会
//BShowSkin TRUE开启皮肤效果 FALSE关闭皮肤效果
STDSKINSAPII JD_Skin(BOOL BSkinShow = TRUE);

//开启或关闭指定控件的皮肤效果 如果一个控件启用了自绘属性 OWNERDRAW 则开启不会成功
//hCtrWnd 是须要开启或关闭的窗口句柄
//BShowSkin TRUE开启皮肤效果 FALSE关闭皮肤显示
STDSKINSAPII JD_CtrlSkin(HWND hCtrWnd, BOOL BSkinShow = TRUE);

//获取主窗口使用的背景色
STDSKINSAPID JD_GetBgColor();

//以下自定义风格是为了MDI框架一些窗口因为系统维护的分隔条无法绘制而设定的 可以同时添加多个 用 | 隔开即可（只对DIALOG有效）
#define CUSTOM_WS_ETCHEDLEFT        0x2             //在窗口客户区左边绘制分隔条（注意：添加此样式意味着皮肤会接管WM_PAINT消息）用户的自定义绘制将失效
#define CUSTOM_WS_ETCHEDRIGHT       0x4             //在窗口客户区右边绘制分隔条（注意：添加此样式意味着皮肤会接管WM_PAINT消息）用户的自定义绘制将失效
#define CUSTOM_WS_ETCHEDTOP         0x8             //在窗口客户区顶部绘制分隔条（注意：添加此样式意味着皮肤会接管WM_PAINT消息）用户的自定义绘制将失效
#define CUSTOM_WS_ETCHEDBOTTOM      0x10            //在窗口客户区底部绘制分隔条（注意：添加此样式意味着皮肤会接管WM_PAINT消息）用户的自定义绘制将失效

//修改内部自定义样式(此样式和窗口样式无关，是皮肤内部自定使用的)
//dwRemove 须要删除的样式
//dwAdd 须要添加的样式
//成功反回TRUE 失败反回FALSE (注意：如果dwRemove和dwAdd同时有值时执行顺序是先减后加)
STDSKINSAPID JD_ModifyStyle(DWORD dwRemove, DWORD dwAdd, HWND hDialog);


#define WND_TYPE_DIADLG            1              //正常窗口
#define WND_TYPE_BUTTON            2              //按钮
#define WND_TYPE_CHECKBOX          3              //多选按钮
#define WND_TYPE_RADIOBTN          4              //单选按钮
#define WND_TYPE_EDIT              5              //文本编辑框
#define WND_TYPE_COMBOBOX          6              //组合框
#define WND_TYPE_COMBOLBOX         7              //ComboLBox组合列表框
#define WND_TYPE_GROUPBOX          8              //GroupBox框选按钮
#define WND_TYPE_STATIC            9              //Static静态文本
#define WND_TYPE_SCROLLBAR        10              //ScrollBar滚动条控件
#define WND_TYPE_LISTBOX          11              //ListBox列表框
#define WND_TYPE_LISTVIEW         12              //ListView列表视图
#define WND_TYPE_HEADER           13              //HeaderControl列表头
#define WND_TYPE_HOT_KEY          14              //HotKey热键
#define WND_TYPE_TRACK_BAR        15              //TrackBar滑块
#define WND_TYPE_UPDOWN           16              //UpDown切换按钮
#define WND_TYPE_PROGRESS         17              //Progress进度条
#define WND_TYPE_TREEVIEW         18              //TreeView树视图控件
#define WND_TYPE_TAB              19              //Tab先项卡
#define WND_TYPE_IPADDR           20              //IPAddress控件
#define WND_TYPE_SYSLINK          21              //SysLink超链接控件
#define WND_TYPE_STATUSBAR        22              //StatusBar状态栏控件
#define WND_TYPE_TOOLBAR          23              //ToolBar工具栏控件
#define WND_TYPE_REBAR            24              //ReBar集中管理控件
#define WND_TYPE_ERASE            25              //BG只绘制背景散件
#define WND_TYPE_MENUS            26              //Menus菜单栏
#define WND_TYPE_MDIDIADLG        27              //MDI框架窗口
#define WND_TYPE_MDICLIENT        28              //MDIClient框架客户窗口
#define WND_TYPE_MDICHILD         29              //MDIClient子窗口
#define WND_TYPE_AFXCONTROLBAR    30              //AfxControlBar停靠窗口
#define WND_TYPE_AFXFRAMEORVIEW   31              //AfxFrameOrView
#define WND_TYPE_AFXMDIFRAME      32              //AfxMDIFrame切割条窗口

//某些窗口系统不能自动识别时可以手动添加进去
//hWnd 须要添加的窗口句柄
//BWndType 窗口类型 参考以上宏定义
//成功反回0 窗口已添加反回1 类型不存在反回2
STDSKINSAPID JD_AddWnd(HWND hWnd, BYTE BWndType);

//把指定窗口移出皮肤接管列表 皮肤程序不再干预窗口任何行为 彻底剥离
//成功反回TRUE 失败反回FALSE
STDSKINSAPID JD_RemoveWnd(HWND hWnd);

//设置指定类型字体大小 成功反回TRUE 失败反回FALSE
//BWndType 窗口类型 参考以上宏定义
//nFontSize 须要设置的字体大小
STDSKINSAPIB JD_SetFontSize(BYTE BWndType, DWORD dwFontSize);
//===========================================================
EXTERN_C_END
