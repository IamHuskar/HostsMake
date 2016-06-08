#include "adddomaindialog.h"
#include "ui_adddomaindialog.h"
#include "hostsxml.h"
#include <QTextBlock>
#include <util.h>

AddDomainDialog::AddDomainDialog(QList<QString>* pDomainList,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDomainDialog)
{
    ui->setupUi(this);
    m_domainList=pDomainList;
    m_textDoc=ui->DomaintextEdit->document();

    //设置每次最多添加的行数
    m_textDoc->setMaximumBlockCount(MAX_DOMAIN_ADD_LINE);
    //设置每行最多输入的字符数。多了就换行
    ui->DomaintextEdit->setLineWrapColumnOrWidth(MAX_DOMAIN_LEN/2);
    ui->DomaintextEdit->setLineWrapMode(QTextEdit::FixedColumnWidth);

    QObject::connect(ui->OkPushButton,SIGNAL(clicked()),this,SLOT(OkClicked()));
    QObject::connect(ui->CancelPushButton,SIGNAL(clicked()),this,SLOT(CancelClicked()));
    //限制最大的输入字符数量
    QObject::connect(ui->DomaintextEdit,SIGNAL(textChanged()),this,SLOT(OnTextChanged()));
}

 void AddDomainDialog::OnTextChanged()
 {
    if(m_textDoc->characterCount()>MAX_DOMAIN_LEN*MAX_DOMAIN_ADD_LINE/2)
    {
        QString qContent=m_textDoc->toPlainText();
        qContent.truncate(MAX_DOMAIN_LEN*MAX_DOMAIN_ADD_LINE/2);
        m_textDoc->setPlainText(qContent);
    }
 }

AddDomainDialog::~AddDomainDialog()
{
    delete ui;
}

void AddDomainDialog::OkClicked()
{
    //把域名数据存放到外部的list里面
    if(!m_domainList)
    {
        this->accept();
        return;
    }
    for (QTextBlock it = m_textDoc->begin(); it != m_textDoc->end(); it = it.next())
    {
         QString Domain=it.text().trimmed();
         if(Domain.length()<MAX_DOMAIN_LEN)
         {
            if(Util::IsValidDomainStr(Domain))
            {
                m_domainList->push_back(Domain);
            }
         }
    }
    this->accept();
}

void AddDomainDialog::CancelClicked()
{
    this->reject();
}
