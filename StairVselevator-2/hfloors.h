﻿#ifndef HFLOORS_H
#define HFLOORS_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QStandardItemModel>

#include "table_view.h"

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
    void on_btn_calculate_clicked();
    void on_btn_confirm_clicked();
    void format_constrain();
    void messagebox_common(const QString &title, const QString &text);
    void clearModel();

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    // new对象，建在堆上
    QStandardItemModel *model;
    QIntValidator      *IntValidator = nullptr;
    TableDelegate      *editDelegate = nullptr;
    QStandardItem      *floors       = nullptr;
    QStandardItem      *stairs       = nullptr;

    int          total_16f;
    bool        *f_16f = nullptr;
    int         *k_16f = nullptr;
    QFont        font_default;
    Ui::hfloors *ui;
};
#endif  // MAINWINDOW_H
