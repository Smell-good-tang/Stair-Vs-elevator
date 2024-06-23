#include "hfloors.h"

#include <bits/stdc++.h>

#include <QGuiApplication>
#include <QIntValidator>
#include <QMessageBox>
#include <QScreen>
#include <QStandardItemModel>
#include <QTimer>

#include "ui_hfloors.h"

hfloors::hfloors(QWidget *parent) : QMainWindow(parent), ui(new Ui::hfloors)
{
    ui->setupUi(this);
    model        = new QStandardItemModel(this);
    editDelegate = new TableDelegate();

    format_constrain();  // 限制控件格式

    QObject::connect(ui->btn_cancel, &QPushButton::clicked, [=] { this->close(); });  // 关闭页面

    font_default = QFont("Microsoft YaHei", 18);
}

hfloors::~hfloors()
{
    delete floors;
    delete stairs;
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            delete model->item(row, col);
        }
    }
    delete model;
    model = nullptr;
    delete editDelegate;
    delete IntValidator;

    // 当数组不再需要时，释放内存
    delete[] f_16f;
    delete[] k_16f;

    // 将指针设置为nullptr，避免出现悬挂指针
    f_16f = nullptr;
    k_16f = nullptr;

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

    // 页面居中
    QRect screenRect = QGuiApplication::primaryScreen()->geometry();  // 获取设备屏幕大小
    // 获取设备像素比
    int screenW = screenRect.width();
    int screenH = screenRect.height();
    this->move((screenW - this->width()) / 2, (screenH - this->height()) / 2);
}

// 统一设置消息框
void hfloors::messagebox_common(const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, title, text, QMessageBox::NoButton, this);
    box->setFont(font_default);
    box->exec();
    delete box;
}

void hfloors::closeEvent(QCloseEvent *event)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, "再见！", "谢谢使用！", QMessageBox::NoButton, this);
    QTimer::singleShot(3000, box, SLOT(accept()));
    box->setFont(font_default);
    box->exec();
    delete box;
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
    if (curtotal != total_16f) {
        messagebox_common("注意！", "您已经更换了总楼层，请先点击按钮2得到可赋值的楼层！");
        return;
    }

    int start_16f = ui->li_2->text().toInt();  // 开始层
    int end_16f   = ui->li_3->text().toInt();  // 目的层

    // 验证输入
    if (ui->li_2->text().isEmpty()) {
        messagebox_common("注意！", "开始层不能为空。");
        return;
    }

    if (ui->li_3->text().isEmpty()) {
        messagebox_common("注意！", "目的层不能为空。");
        return;
    }

    if (start_16f <= 0 || end_16f <= 0) {
        messagebox_common("注意！", "开始层和目的层均不能为负层。");
        return;
    }

    if (start_16f > total_16f || end_16f > total_16f) {
        messagebox_common("注意！", "开始层和目的层均不能超出总层数。");
        return;
    }

    // 初始化上下楼层数组
    k_16f = new int[total_16f + 1];
    f_16f = new bool[total_16f + 1];
    std::fill(f_16f, f_16f + total_16f + 1, false);
    std::fill(k_16f, k_16f + total_16f + 1, 0);

    for (int i = 1; i <= total_16f; ++i) {
        k_16f[i] = model->data(model->index(i - 1, 1)).toInt();  // 为每个楼层指定上/下楼层数

        if (k_16f[i] < 0) {
            messagebox_common("注意！", "每层上下楼层数必须大于等于0。");
            return;
        }

        int max = std::max(i - 1, total_16f - i);  // 最大可移动层数

        if (k_16f[i] > max) {
            messagebox_common("注意！", QString("第").append(QString::number(i)).append("层所赋上下层数超出限度"));
            return;
        }
    }

    // 开始计算最小按键次数
    std::queue<QPair<int, int>> q;
    q.push({start_16f, 0});

    while (!q.empty()) {
        auto [current_floor, press_count] = q.front();
        q.pop();

        if (f_16f[current_floor]) {
            continue;
        }

        f_16f[current_floor] = true;

        if (current_floor == end_16f) {
            messagebox_common("Congratulations!", QString("最少需要按").append(QString::number(press_count)).append("次按钮就能到达指定楼层。"));
            return;
        }

        int next_floor_down = current_floor - k_16f[current_floor];
        if (next_floor_down > 0 && !f_16f[next_floor_down]) {
            q.push({next_floor_down, press_count + 1});
        }

        int next_floor_up = current_floor + k_16f[current_floor];
        if (next_floor_up <= total_16f && !f_16f[next_floor_up]) {
            q.push({next_floor_up, press_count + 1});
        }
    }

    messagebox_common("注意！", "指定条件无法到达。");
}

// 清空表格模型
void hfloors::clearModel()
{
    if (model) {
        model->clear();
    }
}

// 确认总楼层数
void hfloors::on_btn_confirm_clicked()
{
    QString qtotal_16f      = ui->li_1->text();
    int     total_16f_child = qtotal_16f.toInt();

    if (qtotal_16f.isEmpty() || total_16f_child <= 0 || total_16f_child > 162) {
        total_16f = total_16f_child;
        if (qtotal_16f.isEmpty()) {
            messagebox_common("注意！", "总楼层数不能为空。");
        } else if (total_16f_child <= 0) {
            messagebox_common("注意！", "总楼层数必须大于0。");
        } else if (total_16f_child > 162) {
            messagebox_common("注意！", "地球现存最高建筑物楼层数为162层（迪拜塔）。");
        }
        ui->table_floor->hide();
    } else if (total_16f != total_16f_child) {
        total_16f = total_16f_child;

        clearModel();

        model = new QStandardItemModel(total_16f, 2, this);

        ui->table_floor->setItemDelegateForColumn(1, editDelegate);

        model->setHeaderData(0, Qt::Horizontal, "楼层");
        model->setHeaderData(1, Qt::Horizontal, "上下层数");

        ui->table_floor->setModel(model);

        for (int i = 0; i < total_16f; ++i) {
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

// 自适应调整控件字体
void hfloors::resizeEvent(QResizeEvent *event)
{
    (void)event;
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

    ui->label_4->setFont(font_default);
    ui->label_5->setFont(font_default);
    ui->label_6->setFont(font_default);
    ui->btn_calculate->setFont(font_default);
    ui->btn_confirm->setFont(font_default);
    ui->btn_cancel->setFont(font_default);

    ui->table_floor->setFont(font_default);
    ui->table_floor->horizontalHeader()->setFont(font_default);

    ui->li_1->setFont(font_default);
    ui->li_2->setFont(font_default);
    ui->li_3->setFont(font_default);
}

// 处理键盘按压事件
void hfloors::keyPressEvent(QKeyEvent *event)
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
