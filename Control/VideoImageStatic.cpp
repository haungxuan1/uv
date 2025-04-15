// 包含预编译头文件
#include "pch.h"
// 包含 MFC 窗口类的头文件
#include <afxwin.h>
// 包含自定义类 VideoImageStatic 的头文件
#include "VideoImageStatic.h"

// 实现动态类信息，用于 MFC 的运行时类型识别
IMPLEMENT_DYNAMIC(VideoImageStatic, CStatic)

// 开始消息映射，将消息与类的成员函数关联起来
BEGIN_MESSAGE_MAP(VideoImageStatic, CStatic)
	// 处理 WM_CREATE 消息
	ON_WM_CREATE()
	// 处理 WM_PAINT 消息
	ON_WM_PAINT()
	// 处理 WM_DESTROY 消息
	ON_WM_DESTROY()
	// 处理 WM_MOUSEMOVE 消息
	ON_WM_MOUSEMOVE()
	// 处理 WM_LBUTTONDOWN 消息
	ON_WM_LBUTTONDOWN()
	// 处理 WM_LBUTTONUP 消息
	ON_WM_LBUTTONUP()
	// 处理 WM_RBUTTONUP 消息
	ON_WM_RBUTTONUP()
// 结束消息映射
END_MESSAGE_MAP()

// 构造函数
VideoImageStatic::VideoImageStatic()
{
	// 初始化绘制启用标志为 false
	m_bDrawEnable = false;
	// 初始化对比度比例为 1.0f
	m_fContrastRatio = 1.0f;
	// 创建一个纯色画刷，颜色为 RGB(0xf0, 0xf0, 0xf0)
	m_tBrush.CreateSolidBrush(RGB(0xf0, 0xf0, 0xf0));
}

// 析构函数
VideoImageStatic::~VideoImageStatic()
{
	// 如果画布位图对象存在，则删除该对象
	if (m_tBmpCanvas.m_hObject != nullptr) {
		m_tBmpCanvas.DeleteObject();
	}

	// 如果显示矩阵不为空，则释放矩阵资源
	if (!m_tDispMat.empty()) {
		m_tDispMat.release();
	}
}

// 处理 WM_CREATE 消息的函数
int VideoImageStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 调用基类的 OnCreate 函数，如果失败则返回 -1
	if (-1 == CStatic::OnCreate(lpCreateStruct)) {
		return -1;
	}

	// 获取设备上下文
	CDC* ptDC = GetDC();
	if (ptDC != nullptr) {
		// 获取客户区矩形
		GetClientRect(m_tRect);

		// 创建与设备上下文兼容的位图
		m_tBmpCanvas.CreateCompatibleBitmap(ptDC, m_tRect.Width(), m_tRect.Height());

		// 创建与设备上下文兼容的内存设备上下文
		m_tMemDC.CreateCompatibleDC(ptDC);
		// 将位图选入内存设备上下文
		m_tMemDC.SelectObject(&m_tBmpCanvas);

		// 使用画刷填充内存设备上下文的矩形区域
		m_tMemDC.FillRect(&m_tRect, &m_tBrush);

		// 释放设备上下文
		ReleaseDC(ptDC);
	}

	return 0;
}

// 处理 WM_PAINT 消息的函数
void VideoImageStatic::OnPaint()
{
	// 创建一个用于绘制的设备上下文
	CPaintDC tPaintDC(this);

	// 将内存设备上下文的内容拉伸复制到绘制设备上下文
	tPaintDC.StretchBlt(m_tRect.left, m_tRect.top, m_tRect.Width(), m_tRect.Height(),
		&m_tMemDC, 0, 0, m_tRect.Width(), m_tRect.Height(), SRCCOPY);

	// 如果绘制启用且鼠标按下
	if (m_bDrawEnable && m_bMousePress) {
		// 创建一个矩形，由鼠标起始点和结束点确定
		CRect tRect(m_tPointBegin, m_tPointEnd);

		// 创建一个空画刷
		CBrush tBrush;
		tBrush.CreateStockObject(NULL_BRUSH);
		// 选入画刷并保存旧画刷
		CBrush* ptOldBrush = tPaintDC.SelectObject(&tBrush);

		// 创建一个绿色的实线画笔
		CPen tPen(PS_SOLID, 1, RGB(0, 200, 0));
		// 选入画笔并保存旧画笔
		CPen* ptOldPen = tPaintDC.SelectObject(&tPen);

		// 在绘制设备上下文上绘制矩形
		tPaintDC.Rectangle(&tRect);

		// 恢复旧画笔
		tPaintDC.SelectObject(ptOldPen);
		// 恢复旧画刷
		tPaintDC.SelectObject(ptOldBrush);
	}
}

// 处理 WM_DESTROY 消息的函数
void VideoImageStatic::OnDestroy()
{
	// 调用基类的 OnDestroy 函数
	CStatic::OnDestroy();
}

// 重新绘制函数
void VideoImageStatic::reDraw()
{
	// 如果显示矩阵不为空
	if (!m_tDispMat.empty()) {
		// 创建一个目标矩阵
		Mat tDstMat;

		// 根据对比度比例调整矩阵像素值
		cv::convertScaleAbs(m_tDispMat, tDstMat, m_fContrastRatio, 0);

		// 计算每个像素的字节数
		int iPixelBytes = tDstMat.channels() * (tDstMat.depth() + 1);
		// 定义位图信息结构体
		BITMAPINFO tBitmapInfo;

		// 初始化位图信息结构体
		memset((void*)&tBitmapInfo, 0, sizeof(tBitmapInfo));

		// 设置位图信息头的位深度
		tBitmapInfo.bmiHeader.biBitCount = 8 * tDstMat.channels();

		// 设置位图信息头的宽度
		tBitmapInfo.bmiHeader.biWidth = tDstMat.cols;
		// 设置位图信息头的高度，负数表示图像原点在左上角
		tBitmapInfo.bmiHeader.biHeight = -tDstMat.rows;
		// 设置位图信息头的平面数
		tBitmapInfo.bmiHeader.biPlanes = 1;
		// 设置位图信息头的大小
		tBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		// 设置位图信息头的压缩方式为无压缩
		tBitmapInfo.bmiHeader.biCompression = BI_RGB;
		// 设置位图信息头的重要颜色数
		tBitmapInfo.bmiHeader.biClrImportant = 0;
		// 设置位图信息头的使用颜色数
		tBitmapInfo.bmiHeader.biClrUsed = 0;
		// 设置位图信息头的图像大小
		tBitmapInfo.bmiHeader.biSizeImage = 0;
		// 设置位图信息头的水平分辨率
		tBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
		// 设置位图信息头的垂直分辨率
		tBitmapInfo.bmiHeader.biYPelsPerMeter = 0;

		// 获取客户区矩形
		CRect tRect;
		GetClientRect(&tRect);

		// 计算矩阵在客户区的 x 坐标
		m_iMatX = (tRect.Width() - tDstMat.cols) / 2;
		// 计算矩阵在客户区的 y 坐标
		m_iMatY = (tRect.Height() - tDstMat.rows) / 2;

		// 将矩阵数据绘制到内存设备上下文
		::StretchDIBits(m_tMemDC, m_iMatX, m_iMatY, tDstMat.cols, tDstMat.rows,
			0, 0, tDstMat.cols, tDstMat.rows, tDstMat.data,
			&tBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		// 使窗口无效，触发重绘
		Invalidate();
	}
}

// 设置图像的函数
bool VideoImageStatic::setImage(Mat& tImgMat)
{
	// 使用画刷填充内存设备上下文的矩形区域
	m_tMemDC.FillRect(&m_tRect, &m_tBrush);

	// 如果显示矩阵不为空，则释放矩阵资源
	if (!m_tDispMat.empty()) {
		m_tDispMat.release();
	}

	// 如果传入的图像矩阵不为空
	if (!tImgMat.empty()) {
		// 计算图像在 x 方向的缩放比例
		float fScaleX = (float)m_tRect.Width() / tImgMat.cols;
		// 计算图像在 y 方向的缩放比例
		float fScaleY = (float)m_tRect.Height() / tImgMat.rows;

		// 取较小的缩放比例
		m_fScale = (fScaleX < fScaleY) ? fScaleX : fScaleY;

		// 缩放图像矩阵
		resize(tImgMat, m_tDispMat, cv::Size(m_fScale * tImgMat.cols, m_fScale * tImgMat.rows), 0, 0, cv::INTER_AREA);

		// 根据图像通道数进行颜色空间转换
		switch (m_tDispMat.channels()) {
			case 1:		// 单通道图像转换为 BGRA 格式
				cv::cvtColor(m_tDispMat, m_tDispMat, cv::COLOR_GRAY2BGRA);	break;
			case 3:		// 三通道图像转换为 BGRA 格式
				cv::cvtColor(m_tDispMat, m_tDispMat, cv::COLOR_BGR2BGRA);	break;
			default:	// 其他通道数，清空矩阵
				m_tDispMat = Mat();											break;
		}

		// 重新绘制
		reDraw();
	}
	else {
		// 使窗口无效，触发重绘
		Invalidate();
	}

	return true;
}

// 设置背景颜色的函数
void VideoImageStatic::setBgColor(COLORREF uClr)
{
	// 创建一个新的纯色画刷
	m_tBrush.CreateSolidBrush(uClr);
}

// 设置对比度比例的函数
void VideoImageStatic::setContrastRatio(float fContrastRatio)
{
	// 如果对比度比例发生变化
	if (m_fContrastRatio != fContrastRatio) {
		// 更新对比度比例
		m_fContrastRatio = fContrastRatio;

		// 重新绘制
		reDraw();
	}
}

// 设置绘制启用标志的函数
void VideoImageStatic::setDrawEnable(bool bDrawEnable)
{
	// 更新绘制启用标志
	m_bDrawEnable = bDrawEnable;
}

// 获取选择矩形的函数
cv::Rect VideoImageStatic::getSelectRect()
{
	// 初始化选择矩形
	cv::Rect tRect(0, 0, 0, 0);

	// 如果鼠标起始点和结束点不同
	if (m_tPointBegin != m_tPointEnd) {
		// 计算选择矩形的左边界
		int iLeft = (int)((m_tPointBegin.x - m_iMatX) / m_fScale);
		// 计算选择矩形的上边界
		int iTop = (int)((m_tPointBegin.y - m_iMatY) / m_fScale);
		// 计算选择矩形的右边界
		int iRight = (int)((m_tPointEnd.x - m_iMatX) / m_fScale);
		// 计算选择矩形的下边界
		int iBottom = (int)((m_tPointEnd.y - m_iMatY) / m_fScale);

		// 更新选择矩形
		tRect = cv::Rect(iLeft, iTop, iRight - iLeft, iBottom - iTop);
	}

	return tRect;
}

// 处理 WM_MOUSEMOVE 消息的函数
void VideoImageStatic::OnMouseMove(UINT uFlags, CPoint tPoint)
{
	// 如果绘制启用且鼠标按下
	if (m_bDrawEnable && m_bMousePress) {
		// 更新鼠标结束点
		m_tPointEnd = tPoint;

		// 使窗口无效，触发重绘
		Invalidate();
	}
}

// 处理 WM_LBUTTONDOWN 消息的函数
void VideoImageStatic::OnLButtonDown(UINT uFlags, CPoint tPoint)
{
	// 如果绘制启用
	if (m_bDrawEnable) {
		// 设置鼠标按下标志为 true
		m_bMousePress = true;
		// 记录鼠标起始点
		m_tPointBegin = tPoint;
		// 初始化鼠标结束点
		m_tPointEnd = tPoint;

		// 使窗口无效，触发重绘
		Invalidate();
	}
}

// 处理 WM_LBUTTONUP 消息的函数
void VideoImageStatic::OnLButtonUp(UINT uFlags, CPoint tPoint)
{
	// 如果绘制启用
	if (m_bDrawEnable) {
		// 更新鼠标结束点
		m_tPointEnd = tPoint;

		// 设置鼠标按下标志为 false
		m_bMousePress = false;
	}
}

// 处理 WM_RBUTTONUP 消息的函数
void VideoImageStatic::OnRButtonUp(UINT uFlags, CPoint tPoint)
{

}
// 实现 zoomout 函数
void VideoImageStatic::zoomout(double scaleFactor, CPoint center)//参数是缩放比例和中心点
{
    // 获取当前显示的图像
    cv::Mat currentImage = getCurrentImage();

    // 计算新的图像大小
    cv::Size newSize(currentImage.cols * scaleFactor, currentImage.rows * scaleFactor);

    // 缩放图像
    cv::Mat zoomedImage;
    cv::resize(currentImage, zoomedImage, newSize);

    // 计算以鼠标为中心的局部显示区域
    int x = center.x - m_iMatX - (newSize.width - m_tRect.Width()) / 2;//center.x是鼠标的坐标，m_iMatX是图像的偏移量
    int y = center.y - m_iMatY - (newSize.height - m_tRect.Height()) / 2;// 计算局部显示区域的左上角 y 坐标
    x = std::max(0, std::min(x, zoomedImage.cols - m_tRect.Width()));//
    y = std::max(0, std::min(y, zoomedImage.rows - m_tRect.Height()));// 确保 y 坐标在有效范围内

    cv::Rect roi(x, y, m_tRect.Width(), m_tRect.Height());// 定义局部显示区域
    cv::Mat visibleRegion = zoomedImage(roi);// 提取局部显示区域

    // 显示局部可见区域
    displayImage(visibleRegion);
}

// 实现 getCurrentImage 函数
cv::Mat VideoImageStatic::getCurrentImage()
{
    return m_tDispMat;
}

// 定义 VideoImageStatic 类的 getWidth 成员函数，用于获取控件的宽度
int VideoImageStatic::getWidth()
{
    CRect rect;
    GetClientRect(&rect);
    // 返回客户区矩形的宽度
    return rect.Width();
}

// 实现 displayImage 函数
void VideoImageStatic::displayImage(cv::Mat image)
{
    HBITMAP hBitmap = matToBitmap(image);// 调用 matToBitmap 函数将 Mat 转换为 HBITMAP HBITMAP 是 Windows 图形编程中用于表示位图的一种数据类型。
    if (hBitmap) {
        m_tMemDC.FillRect(&m_tRect, &m_tBrush);
        m_tMemDC.SelectObject(&m_tBmpCanvas);

        // 创建 BITMAPINFOHEADER 变量并初始化
        BITMAPINFOHEADER bmiHeader;
        memset(&bmiHeader, 0, sizeof(BITMAPINFOHEADER));
        bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmiHeader.biWidth = image.cols;
        bmiHeader.biHeight = -image.rows; // 负数表示原点在左上角
        bmiHeader.biPlanes = 1;
        bmiHeader.biBitCount = 8 * image.channels();
        bmiHeader.biCompression = BI_RGB;
        bmiHeader.biClrImportant = 0;
        bmiHeader.biClrUsed = 0;
        bmiHeader.biSizeImage = 0;
        bmiHeader.biXPelsPerMeter = 0;
        bmiHeader.biYPelsPerMeter = 0;

        // 创建 BITMAPINFO 变量并关联 BITMAPINFOHEADER
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(BITMAPINFO));
        bmi.bmiHeader = bmiHeader;

        ::StretchDIBits(m_tMemDC, 0, 0, m_tRect.Width(), m_tRect.Height(),
                        0, 0, image.cols, image.rows, image.data,
                        &bmi, DIB_RGB_COLORS, SRCCOPY);

        Invalidate();
        DeleteObject(hBitmap);
    }
}

// 实现 matToBitmap 函数
HBITMAP VideoImageStatic::matToBitmap(cv::Mat image)//转化类型
{
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);// 初始化 BITMAPINFOHEADER 结构体
    bi.biWidth = image.cols;// 设置图像的宽度和高度
    bi.biHeight = -image.rows;  // 负数表示原点在左上角
    bi.biPlanes = 1;// 设置图像的平面数
    bi.biBitCount = 8 * image.channels(); // 根据实际通道数设置
    bi.biCompression = BI_RGB;// 设置图像的压缩方式
    bi.biSizeImage = 0;// 设置图像大小
    bi.biXPelsPerMeter = 0;// 设置水平和垂直分辨率
    bi.biYPelsPerMeter = 0;// 设置水平和垂直分辨率
    bi.biClrUsed = 0;// 设置使用的颜色数
    bi.biClrImportant = 0;// 设置重要的颜色数
    void* pBits;
    HBITMAP hBitmap = CreateDIBSection(NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (hBitmap == NULL) {
        return NULL;
    }

    // 将 Mat 数据复制到 HBITMAP
    memcpy(pBits, image.data, image.total() * image.elemSize());

    return hBitmap;
}