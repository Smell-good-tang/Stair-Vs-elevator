#include <QApplication>
#include <QSharedMemory>

#include "efloors.h"
#ifdef VLD_MODULE
#include "vld.h"
#endif

#ifdef _WIN32
#include "windows.h"
#pragma comment(lib, "user32.lib")  // 添加这行以显式链接 user32.lib
bool activateExistingWindow()
{
    HWND hWnd = FindWindow(nullptr, L"8层运算界面");
    if (hWnd != nullptr) {
        // 找到窗口，激活并恢复它
        if (IsIconic(hWnd)) {
            ShowWindow(hWnd, SW_RESTORE);  // 如果窗口最小化，则恢复它
        }
        SetForegroundWindow(hWnd);
        return true;
    }
    return false;
}
#endif

int main(int argc, char *argv[])
{
    QApplication  a(argc, argv);
    QSharedMemory share;
    share.setKey(QString("Existed_zhihuiyiyuanguanlixitong_single_8_floors"));
    QFont font("Microsoft YaHei", 14);
    // 禁止应用程序同时执行两个及以上
    if (share.attach())  // 如果应用程序已经打开
    {
// 如果可以附加到共享内存，说明已经有一个实例在运行
#ifdef _WIN32
        if (activateExistingWindow()) {
            return 0;  // 已激活窗口，退出应用
        }
#endif
    } else if (share.create(1))  // 如果应用程序没有打开
    {
        efloors e;
        e.show();
        return a.exec();
    }
}
