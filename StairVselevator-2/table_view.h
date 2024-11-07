#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H
#include <QItemDelegate>
#include <QLineEdit>
class TableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TableDelegate(QObject *parent = nullptr);
    ~TableDelegate();

private:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void     setEditorData(QWidget *editor, const QModelIndex &index) const;
    void     setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void     updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // new对象，建在堆上
    QRegularExpression          *regExp          = nullptr;
    QRegularExpressionValidator *regExpValidator = nullptr;
};
#endif  // MAINWINDOW_H
