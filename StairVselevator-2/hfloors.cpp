#include "hfloors.h"

#include <bits/stdc++.h>

#include <QMessageBox>
#include <QModelIndex>
#include <QPair>
#include <QStandardItemModel>
#include <QTimer>

#include "ui_hfloors.h"

hfloors::hfloors(QWidget *parent) : QMainWindow(parent), ui(new Ui::hfloors)
{
    ui->setupUi(this);
    model        = new QStandardItemModel(this);
    editDelegate = new TableDelegate();
    floors       = nullptr;
    stairs       = nullptr;

    format_constrain();  // 限制控件格式

    QObject::connect(ui->btn_cancel, &QPushButton::clicked, [=] { this->close(); });  // 关闭页面

    font_default = QFont("Microsoft YaHei", 18);

    f_16f = nullptr;
    k_16f = nullptr;
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

// 限制格式
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

void hfloors::on_btn_calculate_clicked()
{
    // 还没有输入有效总楼层数，并按按钮2
    if (ui->table_floor->isHidden()) {
        messagebox_common("注意！", "请先输入有效总楼层数并按按钮2！");
    }

    // 已经输入有效总楼层数，并按按钮2
    else if (!ui->table_floor->isHidden()) {
        int curtotal = ui->li_1->text().toInt();  // 获取总楼层数

        // 按按钮2后，没有重新输入不同的新总楼层数
        if (curtotal == total_16f) {
            int start_16f = ui->li_2->text().toInt();  // 开始层数
            int end_16f   = ui->li_3->text().toInt();  // 目的层数

            // 开始层为空
            if (ui->li_2->text().isEmpty()) {
                messagebox_common("注意！", "开始层不能为空。");
            }

            // 目的层为空
            else if (ui->li_3->text().isEmpty()) {
                messagebox_common("注意！", "目的层不能为空。");
            }

            // 开始层或目的层为负值
            else if (start_16f <= 0 || end_16f <= 0) {
                messagebox_common("注意！", "开始层和目的层均不能为负层。");
            }

            // 开始层或目的层超出总层数
            else if (start_16f > total_16f || end_16f > total_16f) {
                messagebox_common("注意！", "开始层和目的层均不能超出总层数。");
            }

            // 数据合规
            else {
                // 当数组不再需要时，释放内存
                delete[] f_16f;
                delete[] k_16f;

                // 将指针设置为nullptr，避免出现悬挂指针
                f_16f = nullptr;
                k_16f = nullptr;

                k_16f = new int[total_16f + 1];
                // int* k_16f = (int*)malloc(total_16f+1);//初始化上下楼层数的数组
                f_16f = new bool[total_16f + 1];
                // bool* f_16f = (bool*)malloc(total_16f+1);//初始化是否到过某层的数组

                // 初始化是否到过楼层的数组
                for (int i = 0; i <= total_16f; i++) {
                    f_16f[i] = false;
                    k_16f[i] = 0;  // 初始化每层上下楼层数数组
                }

                int j = 0;

                // 遍历每层
                for (int i = 1; i <= total_16f; i++) {
                    k_16f[i] = model->data(model->index(i - 1, 1)).toInt();  // 将第i层上下层数依次赋值给数组k
                    // 若第i层上下层数小于0
                    if (k_16f[i] < 0) {
                        messagebox_common("注意！", "每层上下楼层数必须大于等于0。");
                        ++j;
                        break;
                    }

                    int max = i > total_16f - i ? i - 1 : total_16f - i;  // 第i层能上或下楼层数的较大值

                    // 第i层上下楼层数超出楼层上下范围
                    if (k_16f[i] > max) {
                        messagebox_common("注意！", QString("第").append(QString::number(i)).append("层所赋上下层数超出限度"));
                        ++j;
                        break;
                    }
                }

                // 可以运算
                if (j == 0) {
                    int                         result_floor_16f = -1;  // 初始化运算结果
                    int                         start            = 0;
                    std::queue<QPair<int, int>> q;  // 定义队列
                    QPair<int, int> p, t;  // p.first表示第几层，p.second表示按了几次按钮，t负责接受路径执行时p.first和p.second变化后的数值
                    p.first  = start_16f;
                    p.second = 0;       // 给p.first赋初值，就是开始层，给p.second赋值0，就是表示按按钮的次数
                    q.push(p);          // 进入队列
                    while (!q.empty())  // 队列非空
                    {
                        start++;
                        p = q.front();  // 返回到结点在某条路径上的前一个，参与路径选择
                        q.pop();        // 弹出队列的第一个元素，队列长度减1

                        // 如果楼层访问过（如果f[p.first]为true/真）
                        if (f_16f[p.first] == true) {
                            if (p.first == end_16f) {
                                result_floor_16f = p.second;
                                break;
                            }
                            continue;
                        }
                        f_16f[p.first] = true;   // 标记楼层访问过
                        if (p.first == end_16f)  // 到达目标楼层
                        {
                            if (start > 1) {
                                result_floor_16f = p.second;  // 返回需按按钮的次数
                                break;
                            }
                        }

                        // 向下按，若不会下到负楼层，那么执行下面的一条代码段
                        if (p.first - k_16f[p.first] > 0) {
                            t.first  = p.first - k_16f[p.first];  // 当前按按钮之后，到了第几层
                            t.second = p.second + 1;
                            q.push(t);  // 当前按按钮的次数参与接下来的q队列while循环
                        }

                        // 向上按，若不会超出范围，那么执行下面的一条代码段
                        if (p.first + k_16f[p.first] <= total_16f) {
                            t.first  = p.first + k_16f[p.first];  // 当前按按钮之后，到了第几层
                            t.second = p.second + 1;              // 按钮次数加1
                            q.push(t);                            // 当前按按钮的次数参与接下来的q队列while循环
                        }
                    }

                    // 不能到达
                    if (result_floor_16f == -1) {
                        messagebox_common("注意！", "指定条件无法到达。");
                    }

                    // 能到达
                    else {
                        messagebox_common("Congratulations!",
                                          QString("最少需要按").append(QString::number(result_floor_16f)).append("次按钮就能到达指定楼层。"));
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
        ui->table_floor->hide();  // 隐藏上下楼层表格
    } else if (total_16f != total_16f_child) {
        total_16f = total_16f_child;

        for (int row = 0; row < model->rowCount(); ++row) {
            for (int col = 0; col < model->columnCount(); ++col) {
                delete model->item(row, col);
            }
        }
        delete model;
        model = nullptr;

        model = new QStandardItemModel(total_16f, 2);  // 初始化上下楼层表格

        ui->table_floor->setItemDelegateForColumn(1, editDelegate);  // 限制上下楼层表格输入内容

        // 设置表格标题
        model->setHeaderData(0, Qt::Horizontal, "楼层");
        model->setHeaderData(1, Qt::Horizontal, "上下层数");

        ui->table_floor->setModel(model);
        // 遍历初始化上下楼层表格
        for (int i = 0; i < total_16f; i++) {
            floors = new QStandardItem(QString("%1").arg(i + 1));
            stairs = new QStandardItem(QString("%1").arg(0));
            model->setItem(i, 0, floors);
            model->setItem(i, 1, stairs);
            model->item(i, 0)->setEditable(false);
            model->item(i, 0)->setTextAlignment(Qt::AlignCenter);
            model->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        }
        if (ui->table_floor->isHidden()) {
            ui->table_floor->show();  // 显示上下楼层表格
        }
    } else {
        if (ui->table_floor->isHidden()) {
            ui->table_floor->show();  // 显示上下楼层表格
        }
    }
}

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

    // 标签、按钮重设字体
    ui->label_4->setFont(font_default);
    ui->label_5->setFont(font_default);
    ui->label_6->setFont(font_default);
    ui->btn_calculate->setFont(font_default);
    ui->btn_confirm->setFont(font_default);
    ui->btn_cancel->setFont(font_default);

    // 表格重设字体
    ui->table_floor->setFont(font_default);
    ui->table_floor->horizontalHeader()->setFont(font_default);

    // 输入栏重设字体
    ui->li_1->setFont(font_default);
    ui->li_2->setFont(font_default);
    ui->li_3->setFont(font_default);
}

// 键盘按键，控制按钮
void hfloors::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) {
        QWidget *cur_wid = QApplication::focusWidget();
        if (cur_wid == ui->btn_cancel) {
            ui->btn_cancel->clicked();
        } else if (cur_wid == ui->btn_calculate) {
            on_btn_calculate_clicked();
        } else if (cur_wid == ui->btn_confirm) {
            on_btn_confirm_clicked();
        }
    } else if (event->key() == Qt::Key_Escape) {
        ui->btn_cancel->clicked();
    }
}
