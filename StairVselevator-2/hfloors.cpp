#include "hfloors.h"

#include <bits/stdc++.h>

#include <QGuiApplication>
#include <QIntValidator>
#include <QMessageBox>
#include <QScreen>
#include <QStandardItemModel>
#include <QStyle>
#include <QTimer>

#include "ui_hfloors.h"

hfloors::hfloors(QWidget *parent) : QMainWindow(parent), ui(new Ui::hfloors)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    editDelegate = new TableDelegate(this);
    ui->table_floor->setItemDelegateForColumn(1, editDelegate);
    model = new QStandardItemModel(total_162f, 2, this);
    model->setHeaderData(0, Qt::Horizontal, "楼层");
    model->setHeaderData(1, Qt::Horizontal, "上下层数");
    ui->table_floor->setModel(model);
    format_constrain();                                                                     // 限制控件格式
    QObject::connect(ui->btn_cancel, &QPushButton::clicked, this, [=] { this->close(); });  // 关闭页面
    font_default = QFont("Microsoft YaHei", 18);
    foreach (QWidget *widget, this->findChildren<QWidget *>()) {
        widget->setFont(font_default);
    }
}

hfloors::~hfloors()
{
    // 当数组不再需要时，释放内存
    delete[] f_162f;
    delete[] k_162f;
    // 将指针设置为nullptr，避免出现悬挂指针
    f_162f = nullptr;
    k_162f = nullptr;

    delete IntValidator;
    delete ui;
}

// 控制控件格式
void hfloors::format_constrain()
{
    ui->table_floor->hide();                                                          // 隐藏上下楼层表格
    ui->table_floor->horizontalHeader()->setStretchLastSection(true);                 // 设置适应表格内容
    ui->table_floor->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 设置表横向铺满
    ui->table_floor->verticalHeader()->setHidden(true);                               // 隐藏竖直标题

    // 限制输入内容
    IntValidator = new QIntValidator(-999, 999);
    ui->li_1->setValidator(IntValidator);
    ui->li_2->setValidator(IntValidator);
    ui->li_3->setValidator(IntValidator);

    // 适应屏幕实际大小
    QRect screenRect     = QGuiApplication::primaryScreen()->availableGeometry();  // 获取工作区域的几何信息（不包括任务栏）
    int   titleBarHeight = this->style()->pixelMetric(QStyle::PM_TitleBarHeight);  // 获取标题栏高度
    int   screenW        = screenRect.width();
    int   screenH        = screenRect.height();
    int   screen_W_h     = this->width();
    int   screen_H_h     = this->height() + titleBarHeight;
    bool  need_resize    = false;
    if (screen_W_h > screenW) {
        screen_W_h  = screenW;
        need_resize = true;
    }
    if (screen_H_h > screenH) {
        screen_H_h  = screenH - titleBarHeight;
        need_resize = true;
    }
    if (need_resize) {
        this->setMinimumSize(screen_W_h / 5, screen_H_h / 5);
        this->resize(screen_W_h, screen_H_h);
    }

    // 页面居中
    screen_W_h = this->width();
    screen_H_h = this->height() + titleBarHeight;
    this->move((screenW - screen_W_h) / 2, (screenH - screen_H_h) / 2);
}

// 统一设置消息框
void hfloors::messagebox_common(const QString &title, const QString &text)
{
    QMessageBox box(QMessageBox::Information, title, text, QMessageBox::NoButton, this);
    box.setFont(font_default);
    box.exec();
}

// 计算
void hfloors::on_btn_calculate_clicked()
{
    // 检查楼层表是否隐藏
    if (ui->table_floor->isHidden()) {
        messagebox_common("注意！", "请先输入有效总楼层数并按按钮2！");
        return;
    }

    int curtotal = ui->li_1->text().toInt();  // 获得总楼层数

    // 检查楼层总数是否有变化
    if (curtotal != total_162f) {
        messagebox_common("注意！", "您已经更换了总楼层，请先点击按钮2得到可赋值的楼层！");
        return;
    }

    int start_162f = ui->li_2->text().toInt();  // 开始层
    int end_162f   = ui->li_3->text().toInt();  // 目的层

    // 验证输入
    if (ui->li_2->text().isEmpty()) {
        messagebox_common("注意！", "开始层不能为空。");
        return;
    }

    if (ui->li_3->text().isEmpty()) {
        messagebox_common("注意！", "目的层不能为空。");
        return;
    }

    if (start_162f <= 0 || end_162f <= 0) {
        messagebox_common("注意！", "开始层和目的层均不能为负层。");
        return;
    }

    if (start_162f > total_162f || end_162f > total_162f) {
        messagebox_common("注意！", "开始层和目的层均不能超出总层数。");
        return;
    }

    // 初始化上下楼层数组
    k_162f = new int[total_162f + 1];
    f_162f = new bool[total_162f + 1];
    std::fill(f_162f, f_162f + total_162f + 1, false);
    std::fill(k_162f, k_162f + total_162f + 1, 0);

    for (int i = 1; i <= total_162f; ++i) {
        k_162f[i] = model->data(model->index(i - 1, 1)).toInt();  // 为每个楼层指定上/下楼层数

        if (k_162f[i] < 0) {
            messagebox_common("注意！", "每层上下楼层数必须大于等于0。");
            return;
        }

        int max = std::max(i - 1, total_162f - i);  // 最大可移动层数

        if (k_162f[i] > max) {
            messagebox_common("注意！", QString("第").append(QString::number(i)).append("层所赋上下层数超出限度"));
            return;
        }
    }

    // 开始计算最小按键次数
    std::queue<QPair<int, int>> q;
    q.push({start_162f, 0});

    while (!q.empty()) {
        auto [current_floor, press_count] = q.front();
        q.pop();

        if (f_162f[current_floor]) {
            continue;
        }

        f_162f[current_floor] = true;

        if (current_floor == end_162f) {
            messagebox_common("Congratulations!", QString("最少需要按").append(QString::number(press_count)).append("次按钮就能到达指定楼层。"));
            return;
        }

        int next_floor_down = current_floor - k_162f[current_floor];
        if (next_floor_down > 0 && !f_162f[next_floor_down]) {
            q.push({next_floor_down, press_count + 1});
        }

        int next_floor_up = current_floor + k_162f[current_floor];
        if (next_floor_up <= total_162f && !f_162f[next_floor_up]) {
            q.push({next_floor_up, press_count + 1});
        }
    }

    messagebox_common("注意！", "指定条件无法到达。");
}

// 确认总楼层数
void hfloors::on_btn_confirm_clicked()
{
    QString qtotal_162f      = ui->li_1->text();
    int     total_162f_child = qtotal_162f.toInt();

    if (qtotal_162f.isEmpty() || total_162f_child <= 0 || total_162f_child > 162) {
        total_162f = total_162f_child;
        if (qtotal_162f.isEmpty()) {
            messagebox_common("注意！", "总楼层数不能为空。");
        } else if (total_162f_child <= 0) {
            messagebox_common("注意！", "总楼层数必须大于0。");
        } else if (total_162f_child > 162) {
            messagebox_common("注意！", "地球现存最高建筑物楼层数为162层（迪拜塔）。");
        }
        ui->table_floor->hide();
    } else if (total_162f != total_162f_child) {
        total_162f = total_162f_child;

        // 清除所有数据行，但保留标题
        int rowCount = model->rowCount();
        for (int i = rowCount - 1; i >= 0; --i) {
            model->removeRow(i);  // 从末尾删除行，避免索引问题
        }

        for (int i = 0; i < total_162f; ++i) {
            auto floorItem = new QStandardItem(QString::number(i + 1));
            auto stairItem = new QStandardItem("0");

            floorItem->setEditable(false);
            floorItem->setTextAlignment(Qt::AlignCenter);
            stairItem->setTextAlignment(Qt::AlignCenter);

            model->setItem(i, 0, floorItem);
            model->setItem(i, 1, stairItem);
        }
        ui->table_floor->show();
    } else {
        ui->table_floor->show();
    }
}

// 处理键盘按压事件
void hfloors::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Return: {
            QWidget     *cur_wid = QApplication::focusWidget();
            QPushButton *btn     = dynamic_cast<QPushButton *>(cur_wid);
            if (btn) {
                if (btn->isEnabled()) {
                    emit btn->clicked();
                }
                break;
            }
            break;
        }
        case Qt::Key_Escape: {
            if (ui->btn_cancel->isEnabled()) {
                emit ui->btn_cancel->clicked();
            }
            break;
        }
        default:
            QWidget::keyPressEvent(event);
            break;
    }
}
