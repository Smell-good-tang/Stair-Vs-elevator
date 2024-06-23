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
    void on_btn_calculate_clicked();
    void floor_1();
    void on_btn_confirm_clicked();
    void wrong();
    void messagebox_common(const QString &title, const QString &text);
    void format_constrain();

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    int                 total_8f;
    bool               *f_8f = nullptr;
    int                *k_8f = nullptr;
    QFont               font_default;
    QIntValidator      *IntValidator = nullptr;
    QVector<QLabel *>   label_vector;
    QVector<QSpinBox *> floors_vector;
    Ui::efloors        *ui;
};
#endif  // MAINWINDOW_H
