#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include <QStandardItemModel>
#include "hostsxml.h"
#include <QMenu>
#include <QMessageBox>

namespace Ui {
class MainUi;
}



//linux compiler error
extern QString EMPTYSTR;


class MainUi : public QWidget
{
    Q_OBJECT

public:
    explicit MainUi(QWidget *parent = 0);
    ~MainUi();


    enum
    {
        TreeNodeDataRole=Qt::UserRole+1,
        TreeNodeTypeRole=Qt::UserRole+2
    };

    enum
    {
        ListItemDomainRole=Qt::UserRole+1,
        ListItemIpRole=Qt::UserRole+2,
        ListItemGroupRole=Qt::UserRole+3
    };

    enum NODE_TYPE
    {
        NODETYPE_ROOT,
        NODETYPE_IP,
        NODETYPE_GROUP,
        NODETYPE_DOMAIN
    };
    enum
    {
        OP_ADD,
        OP_DEL,
        OP_MODIFY
    };
    typedef struct _RC_NODE_INFO{
      QModelIndex index;
      QString     desc;
      QString     data;
      QString     olddesc;
      QString     olddata;
      int         nodetype;
      int         op;
    } RC_NODE_INFO,*PRC_NODE_INFO;


protected slots:
    //新建xml HOSTS
    void NewHostsClicked();
    //打开xml HOSTS
    void OpenHostsClicked();
    //搜索域名按钮
    void SearchDomainClicked();
    //增加域名
    void AddDomainClicked();
    //删除选定域名
    void DelDomainClicked();
    //保存xml hosts
    void SaveHostsClicked();
    //备份当前系统hosts
    void BackUpHostsClicked();
    //从备份的hosts恢复到系统
    void RestoreHostsClicked();
    //将当前生成的hosts放到系统
    void CopyToSystemClicked();

    //生成hosts
    void MkHostsClicked();

    //搜索框编辑字符
    void SearchDomainTextEdited(const QString & text);

    //triggerSearchDomian
    void TriggerSearchDomain(const QString & dstr);


    //对TreeView当中的某一项进行单击。双击事件
    void TreeItemClicked(const QModelIndex & index);
    void TreeItemDoubleClicked(const QModelIndex & index);
    //tree上右键
    void TreeViewRightClick(const QPoint & pos);


private:
    Ui::MainUi *ui;

    QStandardItemModel TreeModel;
    QStandardItemModel ListModel;

    //主要的XML操作类
    HostsXml m_xml;

    //当前list列表 显示在ip和分组描述
    //QString m_list_ip;
    //QString m_list_ipdesc;
    //QString m_list_group;
    //QString m_list_groupdesc;
    QModelIndex m_selected_tree_item;

    //根据当前的状态判断是否弹出对话框提示保存文件，true需要保存。false不需要保存
    bool BoxNeedSaveFile();
    QString ShowXmlFileDlg(bool save);


    QString ShowHostsFileDlg(bool save,QString defaultFileName=QString());

    //清理掉所有UI相关的资源。相当于从头再来
    void FreeCurrentUiRes();
    //将当前XML DOC里面的内容绘制到UI上
    bool InitXmlUi();



    //打开或者new一个新的工程
    bool OpenOrNewFile(bool openfromfile,QString FileName=EMPTYSTR);

    bool UpdateListView(int NodeType,QString& ipaddr=EMPTYSTR,QString& groupname=EMPTYSTR);
    bool UpdateListView(QDomElement&Node,int NodeType);
    QStandardItem* TreeAddItem(NODE_TYPE type,QString& DisplayText,QString & data,QStandardItem* parent);
    QStandardItem* TreeModifyItem(QStandardItem* item,NODE_TYPE type,QString& DisplayText,QString & data);
    //对于List的操作
    void ListClear();
    void ListAddDomain(QString& ipaddr,QString& groupname,QString& domain);

    //对于tree右键的删除或者添加行为进行响应
    bool AddOrDel_IpOrGroup(RC_NODE_INFO* info);

    //update tree
    bool UpdateTreeView(int NodeType,QString& ipaddr=EMPTYSTR);
    bool UpdateTreeView(const QDomElement&Node,int NodeType);
    void RemoveAllTreeItem();
    QStandardItem* FindTreeItemByUSTR(int NodeType,QString& ipstr=EMPTYSTR,QString& groupstr=EMPTYSTR);


    bool CopyHostsFile(QString& src,QString dest,bool removeifexist=false);
    QString m_sys_host_full_path;
};

//主要是实现鼠标离开后 菜单消失
class LQMenu :public QMenu
{
    Q_OBJECT
public:
    LQMenu(QWidget * parent = 0);
    ~LQMenu();
virtual void leaveEvent(QEvent * event);
    QAction* PopUp();

};
#ifndef MSGBOX
#define MSGBOX(title,info) QMessageBox::information(this,QString::fromLocal8Bit(title),QString::fromLocal8Bit(info));
#endif

//#define MSGBOX(title,content) QMessageBox::information(this,QString::fromLocal8Bit(title),QString::fromLocal8Bit(content))



#endif // MAINUI_H
