#ifndef ADDDOMAINDIALOG_H
#define ADDDOMAINDIALOG_H

#include <QDialog>
#include <QTextDocument>
namespace Ui {
class AddDomainDialog;
}

class AddDomainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDomainDialog(QList<QString>* DomainList,QWidget *parent = 0);
    ~AddDomainDialog();

private:
    Ui::AddDomainDialog *ui;
    QTextDocument* m_textDoc;
    QList<QString>* m_domainList;
private slots:
    void OkClicked();
    void CancelClicked();
    void OnTextChanged();
};

#ifdef _DEBUG
#define MAX_DOMAIN_ADD_LINE 64
#else
//每次最多64行
#define MAX_DOMAIN_ADD_LINE 32
#endif


#endif // ADDDOMAINDIALOG_H
