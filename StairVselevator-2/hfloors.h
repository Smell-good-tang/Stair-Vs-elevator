#ifndef HFLOORS_H
#define HFLOORS_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QStandardItemModel>

#include "./table_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class hfloors;
}
QT_END_NAMESPACE
class hfloors : public QMainWindow
{
    Q_OBJECT

public:
    explicit hfloors(QWidget *parent = nullptr);
    ~hfloors();

private slots:
    void on_btn_calculate_clicked();  // 计算到达指定楼层的最少的按按钮次数
    void on_btn_confirm_clicked();    // 确认总楼层数

protected:
    void keyPressEvent(QKeyEvent *event) override;  // 自定义按键按压事件

private:
    void format_constrain();                                            // 控制控件格式
    void messagebox_common(const QString &title, const QString &text);  // 公用消息框

    // new对象，建在堆上
    QStandardItemModel *model        = nullptr;  // 上下楼层数表格加载的模型
    QIntValidator      *IntValidator = nullptr;  // 总楼层数、开始层数、目的层数的数据限制
    TableDelegate      *editDelegate = nullptr;  // 上下楼层数数据限制
    bool               *f_162f       = nullptr;  // 储存楼层是否已到达
    int                *k_162f       = nullptr;  // 储存楼层上下楼层数
    Ui::hfloors        *ui;

    int   total_162f = 0;  // 总楼层数
    QFont font_default;    // 默认字体
};
#endif  // MAINWINDOW_H
