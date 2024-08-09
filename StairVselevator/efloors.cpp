#include "efloors.h"

#include <bits/stdc++.h>

#include <QGuiApplication>
#include <QLabel>
#include <QMessageBox>
#include <QPair>
#include <QProcess>
#include <QStyle>
#include <QTimer>

#include "ui_efloors.h"

efloors::efloors(QWidget *parent) : QMainWindow(parent), ui(new Ui::efloors)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    label_vector  = {ui->label, ui->label_2, ui->label_4, ui->label_5, ui->label_6};
    floors_vector = {ui->spin_4, ui->spin_5, ui->spin_6, ui->spin_7, ui->spin_8, ui->spin_9, ui->spin_10, ui->spin_11};
    wrong();
    format_constrain();                                                                     // 设置控件格式
    QObject::connect(ui->btn_cancel, &QPushButton::clicked, this, [=] { this->close(); });  // 关闭页面
    font_default = QFont("Microsoft YaHei", 18);
    total_8f     = 0;
}

efloors::~efloors()
{
    delete IntValidator;
    label_vector.clear();
    floors_vector.clear();

    // 当数组不再需要时，释放内存
    delete[] f_8f;
    delete[] k_8f;

    // 将指针设置为nullptr，避免出现悬挂指针
    f_8f = nullptr;
    k_8f = nullptr;

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

    // 适应屏幕实际大小
    QRect screenRect     = QGuiApplication::primaryScreen()->availableGeometry();  // 获取工作区域的几何信息（不包括任务栏）
    int   titleBarHeight = this->style()->pixelMetric(QStyle::PM_TitleBarHeight);  // 获取标题栏高度
    int   screenW        = screenRect.width();
    int   screenH        = screenRect.height();
    int   screen_W_e     = this->width();
    int   screen_H_e     = this->height() + titleBarHeight;
    bool  need_resize    = false;
    if (screen_W_e > screenW) {
        screen_W_e  = screenW;
        need_resize = true;
    }
    if (screen_H_e > screenH) {
        screen_H_e  = screenH - titleBarHeight;
        need_resize = true;
    }
    if (need_resize) {
        this->setMinimumSize(screen_W_e / 5, screen_H_e / 5);
        this->resize(screen_W_e, screen_H_e);
    }

    // 页面居中
    screen_W_e = this->width();
    screen_H_e = this->height() + titleBarHeight;
    this->move((screenW - screen_W_e) / 2, (screenH - screen_H_e) / 2);
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

void efloors::closeEvent(QCloseEvent *event)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, "再见！", "谢谢使用！", QMessageBox::NoButton, this);
    QTimer::singleShot(3000, box, SLOT(accept()));
    box->setFont(font_default);
    box->exec();
    delete box;
}

// 计算
void efloors::on_btn_calculate_clicked()
{
    // 检查标签是否隐藏，表明尚未输入楼层总数
    if (ui->label->isHidden()) {
        messagebox_common("注意！", "请先输入有效总楼层数并按按钮2！");
        return;
    }

    int curtotal = ui->li_1->text().toInt();

    // 检查输入的总楼层数是否与之前输入的相同
    if (curtotal != total_8f) {
        messagebox_common("注意！", "您已经更换了总楼层，请先点击按钮2得到可赋值的楼层！");
        return;
    }

    int start_8f = ui->li_2->text().toInt();
    int end_8f   = ui->li_3->text().toInt();

    if (ui->li_2->text().isEmpty() || ui->li_3->text().isEmpty()) {
        messagebox_common("注意！", "开始层和目的层不能为空。");
        return;
    }

    if (start_8f <= 0 || end_8f <= 0) {
        messagebox_common("注意！", "开始层和目的层均不能为负层。");
        return;
    }

    if (start_8f > total_8f || end_8f > total_8f) {
        messagebox_common("注意！", "开始层和目的层均不能超出总层数。");
        return;
    }

    // 释放和重塑楼层数据
    delete[] f_8f;
    delete[] k_8f;
    f_8f = new bool[total_8f + 1]{false};
    k_8f = new int[total_8f + 1]{0};

    // 验证上下楼层值
    for (int i = 1; i <= total_8f; ++i) {
        k_8f[i] = floors_vector[i - 1]->value();
        if (k_8f[i] < 0) {
            messagebox_common("注意！", "每层上下楼层数必须大于等于0。");
            return;
        }

        int max = std::max(i - 1, total_8f - i);
        if (k_8f[i] > max) {
            messagebox_common("注意！", QString("第").append(QString::number(i)).append("层所赋上下层数超出限度"));
            return;
        }
    }

    // 到达目的层的最少按按钮次数
    std::queue<QPair<int, int>> q;
    q.push({start_8f, 0});

    while (!q.empty()) {
        auto [current_floor, press_count] = q.front();
        q.pop();

        if (f_8f[current_floor]) {
            continue;
        }

        f_8f[current_floor] = true;

        if (current_floor == end_8f) {
            messagebox_common("Congratulations!", QString("最少需要按").append(QString::number(press_count)).append("次按钮就能到达指定楼层。"));
            return;
        }

        int next_floor_down = current_floor - k_8f[current_floor];
        if (next_floor_down > 0 && !f_8f[next_floor_down]) {
            q.push({next_floor_down, press_count + 1});
        }

        int next_floor_up = current_floor + k_8f[current_floor];
        if (next_floor_up <= total_8f && !f_8f[next_floor_up]) {
            q.push({next_floor_up, press_count + 1});
        }
    }

    messagebox_common("注意！", "指定条件无法到达。");
}

// 控制可显示的上下楼层
void efloors::floor_1()
{
    int total_8f_cur = ui->li_1->text().toInt();
    if (total_8f < total_8f_cur) {
        for (int i = total_8f; i < total_8f_cur; i++) {
            floors_vector[i]->show();
        }
    } else if (total_8f > total_8f_cur) {
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

// 自适应调整控件字体
void efloors::resizeEvent(QResizeEvent *event)
{
    int width  = this->width();
    int height = this->height();

    qreal screen_dpi = QGuiApplication::primaryScreen()->devicePixelRatio();  // 获取主屏幕的缩放比例

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
    font_default.setPointSize(origin_count);

    for (QLabel *label : label_vector) {
        label->setFont(font_default);
    }
    ui->btn_calculate->setFont(font_default);
    ui->btn_confirm->setFont(font_default);
    ui->btn_cancel->setFont(font_default);
}

// 处理键盘按压事件
void efloors::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            QWidget *cur_wid = QApplication::focusWidget();
            if (cur_wid == ui->btn_cancel) {
                ui->btn_cancel->click();
            } else if (cur_wid == ui->btn_calculate) {
                on_btn_calculate_clicked();
            } else if (cur_wid == ui->btn_confirm) {
                on_btn_confirm_clicked();
            }
            break;
        }
        case Qt::Key_Escape: {
            ui->btn_cancel->click();
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
