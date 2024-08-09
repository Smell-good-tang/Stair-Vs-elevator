#include <QApplication>
#include <QScreen>
#include <QStyle>

#include "SingleInstanceApp.h"
#include "efloors.h"
#ifdef VLD_MODULE
#include "vld.h"
#endif

#include "Windows.h"
#pragma comment(lib, "user32.lib")  // 添加这行以显式链接 user32.lib

int main(int argc, char *argv[])
{
    QApplication       a(argc, argv);
    efloors           *e              = new efloors();
    SingleInstanceApp *singleInstance = new SingleInstanceApp(e);
    QObject::connect(singleInstance, &SingleInstanceApp::showExistingWindow, e, [=]() {
        if (!e->isHidden()) {
            HWND  hForeWnd = NULL;
            HWND  hWnd     = HWND(e->winId());
            DWORD dwForeID;
            DWORD dwCurID;

            hForeWnd = GetForegroundWindow();
            dwCurID  = GetCurrentThreadId();
            dwForeID = GetWindowThreadProcessId(hForeWnd, NULL);
            AttachThreadInput(dwCurID, dwForeID, TRUE);
            ShowWindow(hWnd, SW_SHOWNORMAL);
            SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            SetForegroundWindow(hWnd);
            AttachThreadInput(dwCurID, dwForeID, FALSE);
        } else {
            e->activateWindow();
            e->show();
        }
    });
    e->show();
    return a.exec();
}
