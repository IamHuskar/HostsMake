#ifndef ADDNODEDLG_H
#define ADDNODEDLG_H

#include <QDialog>
#include <mainui.h>

namespace Ui {
class AddNodeDlg;
}

class AddNodeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddNodeDlg(MainUi::RC_NODE_INFO* rcinfo,QWidget *parent = 0);
    ~AddNodeDlg();

private:
    Ui::AddNodeDlg *ui;
    MainUi::RC_NODE_INFO* m_rcinfo;
private slots:
    void OkClicked();
    void CancelClicked();
};

#endif // ADDNODEDLG_H
