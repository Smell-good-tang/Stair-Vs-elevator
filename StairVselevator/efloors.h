#ifndef EFLOORS_H
#define EFLOORS_H

#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QSpinBox>
QT_BEGIN_NAMESPACE
namespace Ui {
class efloors;
}
QT_END_NAMESPACE
class efloors : public QMainWindow
{
    Q_OBJECT

public:
    explicit efloors(QWidget *parent = nullptr);
    ~efloors();

private slots:
    void on_btn_calculate_clicked();  // 计算到达指定楼层的最少的按按钮次数
    void on_btn_confirm_clicked();    // 确认总楼层数

protected:
    void keyPressEvent(QKeyEvent *event) override;  // 自定义按键按压事件

private:
    void hide_and_initialize();                                         // 隐藏并初始化控件
    void format_constrain();                                            // 控制控件格式
    void floor_able_to_be_seen();                                       // 可以输入上下楼层数的层
    void messagebox_common(const QString &title, const QString &text);  // 公用消息框

private:
    // new对象，建在堆上
    QIntValidator *IntValidator = nullptr;  // 输入数据限制
    bool          *f_8f         = nullptr;  // 储存楼层是否已到达
    int           *k_8f         = nullptr;  // 储存楼层上下楼层数
    Ui::efloors   *ui;

    int                 total_8f = 0;   // 总楼层数
    QFont               font_default;   // 默认字体
    QVector<QSpinBox *> floors_vector;  // 储存上下楼层数数字盒
};
#endif  // MAINWINDOW_H
