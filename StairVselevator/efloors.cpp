#include "efloors.h"

#include <bits/stdc++.h>

#include <QLabel>
#include <QMessageBox>
#include <QPair>
#include <QProcess>
#include <QTimer>

#include "ui_efloors.h"

efloors::efloors(QWidget *parent) : QMainWindow(parent), ui(new Ui::efloors)
{
    ui->setupUi(this);
    label_vector  = {ui->label, ui->label_2, ui->label_4, ui->label_5, ui->label_6};
    floors_vector = {ui->spin_4, ui->spin_5, ui->spin_6, ui->spin_7, ui->spin_8, ui->spin_9, ui->spin_10, ui->spin_11};
    wrong();
    format_constrain();  // 设置控件格式

    QObject::connect(ui->btn_cancel, &QPushButton::clicked, [=] { this->close(); });  // 关闭页面
    font_default = QFont("华文中宋", 18);

    total_8f = 0;
}

efloors::~efloors()
{
    delete IntValidator;

    label_vector.clear();
    floors_vector.clear();

    delete ui;
}

// 设置控件格式
void efloors::format_constrain()
{
    // 限制输入范围
    IntValidator = new QIntValidator(-999, 999);
    ui->li_1->setValidator(IntValidator);
    ui->li_2->setValidator(IntValidator);
    ui->li_3->setValidator(IntValidator);

    // 页面居中
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();  // 获取设备屏幕大小
    // 获取设备像素比
    int screenW = screenRect.width();
    int screenH = screenRect.height();
    this->move((screenW - this->width()) / 2, (screenH - this->height()) / 2);
}

// 根据系统DPI重设字体
const QFont efloors::font_resize(const int &width, const int &height, const int &screen_dpi)
{
    int origin_count = 14;
    if (width <= 1000 || height <= 730) {
        origin_count += 2;
    } else {
        origin_count += 4;
    }
    if (1.1 < screen_dpi && screen_dpi <= 1.35) {
        origin_count -= 2;
    } else if (1.35 < screen_dpi && screen_dpi <= 1.55) {
        origin_count -= 4;
    } else if (screen_dpi > 1.55) {
        origin_count -= 6;
    }
    QFont font("Microsoft YaHei", origin_count);
    return font;
}

// 重设标签字体
void efloors::refont(QFont a)
{
    for (QLabel *label : label_vector) {
        label->setFont(a);
    }
    ui->btn_caculate->setFont(a);
    ui->btn_confirm->setFont(a);
    ui->btn_cancel->setFont(a);
}

// 隐藏控件
void efloors::wrong()
{
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();

    for (QSpinBox *spinbox : floors_vector) {
        spinbox->setValue(0);  // 重置每个楼层能上下楼层的数值
        spinbox->close();
    }
}

// 统一设置消息框
void efloors::messagebox_common(const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, title, text, QMessageBox::NoButton, this);
    box->setFont(font_default);
    box->exec();
    delete box;
}

// 计算
void efloors::on_btn_caculate_clicked()
{
    // 还没有输入有效总楼层数，并按按钮2
    if (ui->label->isHidden()) {
        messagebox_common("注意！", "请先输入有效总楼层数并按按钮2！");
    }

    // 已经输入有效总楼层数，并按按钮2
    else if (!ui->label->isHidden()) {
        int curtotal = ui->li_1->text().toInt();

        // 按按钮2后，没有重新输入不同的新总楼层数
        if (curtotal == total_8f) {
            int start_8f = ui->li_2->text().toInt();  // 获取开始层层数
            int end_8f   = ui->li_3->text().toInt();  // 获取目的层层数

            if (ui->li_2->text().isEmpty()) {
                messagebox_common("注意！", "开始层不能为空。");
            } else if (ui->li_3->text().isEmpty()) {
                messagebox_common("注意！", "目的层不能为空。");
            } else if (start_8f <= 0 || end_8f <= 0) {
                messagebox_common("注意！", "开始层和目的层均不能为负层。");
            } else if (start_8f > total_8f || end_8f > total_8f) {
                messagebox_common("注意！", "开始层和目的层均不能超出总层数。");
            } else {
                k_8f = new int[total_8f + 1];
                f_8f = new bool[total_8f + 1];

                // 初始化是否到过楼层的数组
                for (int i = 0; i <= total_8f; i++) {
                    f_8f[i] = false;
                    k_8f[i] = 0;  // 初始化每层上下楼层数数组
                }

                int j = 0;

                // 将第i层上下层数依次赋值给数组k
                for (int i = 1; i <= total_8f; i++) {
                    k_8f[i] = floors_vector[i - 1]->value();
                    // 若第i层上下层数小于0
                    if (k_8f[i] < 0) {
                        messagebox_common("注意！", "每层上下楼层数必须大于等于0。");
                        ++j;
                        break;
                    }

                    int max = i > total_8f - i ? i - 1 : total_8f - i;  // 第i层能上或下楼层数的较大值

                    // 第i层上下楼层数超出楼层上下范围
                    if (k_8f[i] > max) {
                        messagebox_common("注意！", QString("第").append(QString::number(i)).append("层所赋上下层数超出限度"));
                        ++j;
                        break;
                    }
                }

                // 可以运算
                if (j == 0) {
                    int                         result_floor_8f = -1;
                    int                         start           = 0;
                    std::queue<QPair<int, int>> q;  // 定义队列
                    QPair<int, int> p, t;  // p.first表示第几层，p.second表示按了几次按钮，t负责接受路径执行时p.first和p.second变化后的数值
                    p.first  = start_8f;
                    p.second = 0;       // 给p.first赋初值，就是开始层，给p.second赋值0，就是表示按按钮的次数
                    q.push(p);          // 进入队列
                    while (!q.empty())  // 队列非空
                    {
                        start++;
                        p = q.front();                // 返回到结点在某条路径上的前一个，参与路径选择
                        q.pop();                      // 弹出队列的第一个元素，队列长度减1
                        if (f_8f[p.first] == true) {  // 如果楼层访问过（如果f[p.first]为true/真）
                            if (p.first == end_8f) {
                                result_floor_8f = p.second;
                                break;
                            }
                            continue;
                        }
                        f_8f[p.first] = true;   // 标记楼层访问过
                        if (p.first == end_8f)  // 到达目标楼层
                        {
                            if (start > 1) {
                                result_floor_8f = p.second;  // 返回需按按钮的次数
                                break;
                            }
                        }

                        // 向下按，若不会下到负楼层，那么执行下面的一条代码段
                        if (p.first - k_8f[p.first] > 0) {
                            t.first  = p.first - k_8f[p.first];  // 当前按按钮之后，到了第几层
                            t.second = p.second + 1;
                            q.push(t);  // 当前按按钮的次数参与接下来的q队列while循环
                        }

                        // 向上按，若不会超出范围，那么执行下面的一条代码段
                        if (p.first + k_8f[p.first] <= total_8f) {
                            t.first  = p.first + k_8f[p.first];  // 当前按按钮之后，到了第几层
                            t.second = p.second + 1;             // 按钮次数加1
                            q.push(t);                           // 当前按按钮的次数参与接下来的q队列while循环
                        }
                    }
                    if (result_floor_8f == -1) {
                        messagebox_common("注意！", "指定条件无法到达。");
                    } else {
                        messagebox_common("Congratulations!",
                                          QString("最少需要按").append(QString::number(result_floor_8f)).append("次按钮就能到达指定楼层。"));
                    }
                }
            }
        }

        // 按按钮2后，重新输入了不同的新总楼层数
        else {
            messagebox_common("注意！", "您已经更换了总楼层，请先点击按钮2得到可赋值的楼层！");
        }
    }
}

// 重新选定新的总楼层数，改变对应控件状态
void efloors::floor_1()
{
    int total_8f_cur = ui->li_1->text().toInt();
    if (total_8f < total_8f_cur) {
        // 显示增加了的上下楼层数控件
        for (int i = total_8f; i < total_8f_cur; i++) {
            floors_vector[i]->show();
        }
    } else if (total_8f > total_8f_cur) {
        // 隐藏多余的上下楼层数控件
        for (int i = total_8f_cur; i < total_8f; i++) {
            floors_vector[i]->close();
        }
    }
}

// 确认总楼层数
void efloors::on_btn_confirm_clicked()
{
    QString qtotal_8f      = ui->li_1->text();
    int     total_8f_child = qtotal_8f.toInt();
    if (qtotal_8f.isEmpty() || total_8f_child <= 0 || total_8f_child > 8) {
        total_8f = 0;
        if (qtotal_8f.isEmpty()) {
            messagebox_common("注意！", "总楼层数不能为空。");
        } else if (total_8f_child <= 0) {
            messagebox_common("注意！", "总楼层数必须大于0。");
        } else if (total_8f_child > 8) {
            messagebox_common("注意！", "该版本运算程序最多运算总楼层数为8层！");
        }
        wrong();
    } else if (total_8f != total_8f_child) {
        floor_1();
        total_8f = total_8f_child;
        ui->label_2->setText(QString("第").append(QString::number(total_8f).append("层")));
        ui->label->show();
        ui->label_2->show();
        ui->label_3->show();
    }
}

// 控件字体自适应窗口缩放
void efloors::resizeEvent(QResizeEvent *event)
{
    int Width  = this->width();
    int Height = this->height();

    // 获取主屏幕的缩放比例
    qreal scaleFactor = QGuiApplication::primaryScreen()->devicePixelRatio();
    refont(font_resize(Width, Height, scaleFactor));
}

void efloors::closeEvent(QCloseEvent *event)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, "再见！", "谢谢使用！", QMessageBox::NoButton, this);
    QTimer::singleShot(3000, box, SLOT(accept()));
    box->setFont(font_default);
    box->exec();
    delete box;
}

// 键盘按键，控制按钮
void efloors::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        QWidget *cur_wid = QApplication::focusWidget();
        if (cur_wid == ui->btn_cancel) {
            ui->btn_cancel->clicked();
        } else if (cur_wid == ui->btn_caculate) {
            on_btn_caculate_clicked();
        } else if (cur_wid == ui->btn_confirm) {
            on_btn_confirm_clicked();
        }
    } else if (event->key() == Qt::Key_Escape) {
        ui->btn_cancel->clicked();
    }
}
