#include <QApplication>
#include <QSharedMemory>

#include "SingleInstanceApp.h"
#include "hfloors.h"
#ifdef VLD_MODULE
#include "vld.h"
#endif

#include "Windows.h"
#pragma comment(lib, "user32.lib")  // 添加这行以显式链接 user32.lib

int main(int argc, char *argv[])
{
    QApplication       a(argc, argv);
    hfloors           *h              = new hfloors();
    SingleInstanceApp *singleInstance = new SingleInstanceApp(h);
    QObject::connect(singleInstance, &SingleInstanceApp::showExistingWindow, h, [=]() {
        if (!h->isHidden()) {
            HWND  hForeWnd = NULL;
            HWND  hWnd     = HWND(h->winId());
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
            h->activateWindow();
            h->show();
        }
    });
    h->show();
    return a.exec();
}
