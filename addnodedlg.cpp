#include "addnodedlg.h"
#include "ui_addnodedlg.h"
#include "util.h"

AddNodeDlg::AddNodeDlg(MainUi::RC_NODE_INFO* rcinfo,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNodeDlg)
{
    ui->setupUi(this);
    m_rcinfo=rcinfo;

    //限制描述的长度,其他的随便输入
    ui->DescLineEdit->setMaxLength(MAX_DESC_LEN);
    ui->DescLabel->setText(rcinfo->desc);

    //如果是ip设置IP的过滤器 在root上面点是增加IP。
    if(rcinfo->nodetype==MainUi::NODETYPE_ROOT||(rcinfo->nodetype==MainUi::NODETYPE_IP&&rcinfo->op==MainUi::OP_MODIFY))
    {
        ui->DataLineEdit->setValidator(new Ipv4Validator(this));
    }
    else
    {
        //其余的都设置普通的英文描述符
        ui->DataLineEdit->setValidator(new enDescValidator(this));
    }
    ui->DataLabel->setText(rcinfo->data);

    //清空输入数据。准备作为输出数据
    rcinfo->data.clear();
    rcinfo->desc.clear();

    if(rcinfo->op==MainUi::OP_MODIFY)
    {
        ui->DataLineEdit->setText(rcinfo->olddata);
        ui->DescLineEdit->setText(rcinfo->olddesc);
    }

    QObject::connect(ui->OkPushButton,SIGNAL(clicked()),this,SLOT(OkClicked()));
    QObject::connect(ui->CancelPushButton,SIGNAL(clicked()),this,SLOT(CancelClicked()));

}

void AddNodeDlg::OkClicked()
{
    QString qdesc=ui->DescLineEdit->text().trimmed();
    QString qdata=ui->DataLineEdit->text().trimmed();

    do
    {
        if(qdesc.length()==0)
        {
            MSGBOX("警告","描述不能为空");
            break;
        }
        if(qdata.length()==0)
        {
            if(m_rcinfo->nodetype==MainUi::NODETYPE_ROOT)
            {
                MSGBOX("警告","IP不能为空");
            }
            else
              MSGBOX("警告","英文描述不能为空");
            break;
        }

        if(m_rcinfo->nodetype==MainUi::NODETYPE_ROOT)
        {
            if(!Util::IsValidIpv4Str(qdata))
            {
                MSGBOX("警告","请输入合法的IP地址");
                break;
            }
        }
        m_rcinfo->desc=qdesc;
        m_rcinfo->data=qdata;
        //返回
        this->accept();
    }while(false);

}

void AddNodeDlg::CancelClicked()
{
    this->reject();
}


AddNodeDlg::~AddNodeDlg()
{
    delete ui;
}
