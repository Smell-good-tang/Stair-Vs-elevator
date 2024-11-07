#include "./table_view.h"

TableDelegate::TableDelegate(QObject *parent) : QItemDelegate(parent)
{
    regExp          = new QRegularExpression();
    regExpValidator = new QRegularExpressionValidator(this);
}

TableDelegate::~TableDelegate() { delete regExp; };

QWidget *TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    regExp->setPattern("^-?([0-9]{1,3})$");
    regExpValidator->setRegularExpression(*regExp);
    QLineEdit *editor = new QLineEdit(parent);
    editor->setValidator(regExpValidator);
    return editor;
}

void TableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString    text     = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    lineEdit->setText(text);
}

void TableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    QString    text     = lineEdit->text();
    model->setData(index, text, Qt::EditRole);
}

void TableDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
