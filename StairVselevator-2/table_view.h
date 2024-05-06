#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H
#include <QItemDelegate>
#include <QLineEdit>
class TableDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TableDelegate(QObject *parent = nullptr) : QItemDelegate(parent) {}
    ~TableDelegate()
    {
        delete regExp;
        delete regExpValidator;
    };
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        regExp->setPattern("^-?([0-9]{1,3})$");
        regExpValidator->setRegularExpression(*regExp);
        QLineEdit *editor = new QLineEdit(parent);
        editor->setValidator(regExpValidator);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QString    text     = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
        lineEdit->setText(text);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
        QString    text     = lineEdit->text();
        model->setData(index, text, Qt::EditRole);
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const { editor->setGeometry(option.rect); }

private:
    // new对象，建在堆上
    QRegularExpression          *regExp          = new QRegularExpression();
    QRegularExpressionValidator *regExpValidator = new QRegularExpressionValidator();
};
#endif  // MAINWINDOW_H
