#ifndef HFLOORS_H
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
    void        on_btn_caculate_clicked();
    void        on_btn_confirm_clicked();
    void        refont(const QFont &a);
    void        refont2(const QFont &b);
    void        refont1(const QFont &c);
    void        format_constrain();
    const QFont font_resize(const int &width, const int &height, const int &screen_dpi);
    void        messagebox_common(const QString &title, const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    // new对象，建在堆上
    QStandardItemModel *model;
    int                 total_16f;
    QIntValidator      *IntValidator;
    TableDelegate      *editDelegate;
    QStandardItem      *floors;
    QStandardItem      *stairs;

    bool        *f_16f;
    int         *k_16f;
    QFont        font_default;
    Ui::hfloors *ui;
};
#endif  // MAINWINDOW_H
