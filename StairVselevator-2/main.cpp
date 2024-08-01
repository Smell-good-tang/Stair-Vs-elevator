#include <QApplication>
#include <QSharedMemory>

#include "SingleInstanceApp.h"
#include "hfloors.h"
#ifdef VLD_MODULE
#include "vld.h"
#endif

#ifdef _WIN32
#include "windows.h"
#pragma comment(lib, "user32.lib")  // 添加这行以显式链接 user32.lib
void mouse_cursor(QWidget *widget)
{
    // 获取全局鼠标位置
    QPoint globalPos_mouse = QCursor::pos();
    // 获取窗口在屏幕上的位置
    QPoint globalPos      = widget->mapToGlobal(QPoint(0, 0));
    int    titleBarHeight = widget->geometry().top() - globalPos.y();

    // 获取主屏幕的缩放比例
    QScreen *screen        = QApplication::primaryScreen();
    qreal    scalingFactor = screen->devicePixelRatio();

    // 调整为实际的屏幕坐标
    int x = static_cast<int>(globalPos.x() * scalingFactor + widget->width() / 2);
    int y = static_cast<int>(globalPos.y() * scalingFactor - titleBarHeight / 2);

    // 调整为实际的屏幕坐标
    int x_ori = static_cast<int>(globalPos_mouse.x() * scalingFactor);
    int y_ori = static_cast<int>(globalPos_mouse.y() * scalingFactor);

    RECT mainWinRect;  // RECT在windef.h中被定义
    mainWinRect.left   = (LONG)widget->geometry().left() - 0.00001;
    mainWinRect.right  = (LONG)widget->geometry().right() + 0.00001;
    mainWinRect.top    = (LONG)widget->geometry().top() + 0.00001;
    mainWinRect.bottom = (LONG)widget->geometry().bottom() - 0.00001;

    ClipCursor(&mainWinRect);  // 这是Windows API

    // 设置鼠标位置
    SetCursorPos(x, y);

    mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);  // 鼠标状态还原
    // 模拟鼠标左键按下和释放事件
    mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);

    ClipCursor(nullptr);
    // 鼠标位置还原
    SetCursorPos(x_ori, y_ori);
}
#endif

int main(int argc, char *argv[])
{
    QApplication       a(argc, argv);
    hfloors           *h              = new hfloors();
    SingleInstanceApp *singleInstance = new SingleInstanceApp(h);
    QObject::connect(singleInstance, &SingleInstanceApp::showExistingWindow, h, [=]() {
        if (h->isMinimized()) {
            h->showNormal();
        }
        if (h->isHidden()) {
            h->showNormal();
        }
        SetWindowPos(HWND(h->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetWindowPos(HWND(h->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        mouse_cursor(h);
    });
    h->show();
    return a.exec();
}
