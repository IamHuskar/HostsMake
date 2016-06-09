#include "mainui.h"
#include "ui_mainui.h"
#include "logger.h"
#include "util.h"
#include <QFileDialog>
#include <QMessageBox>
#include "adddomaindialog.h"
#include "addnodedlg.h"

#ifndef DEFAULT_VAR
#define DEFAULT_VAR

#define DIR_XMLFILE             "/xml/"
#define DIR_HOSTS_BACKUP        "/backup/"
#define DEFAULT_XML_FILENAME    "hosts.xml"
#define DEFAULT_MAKE_HOSTS_NAME "hosts.hosts"


#ifndef _WIN32
#define SYSTEM_HOSTS_SUFFIX "etc/hosts"
#else
#define SYSTEM_HOSTS_SUFFIX "windows/system32/drivers/etc/hosts"
#endif




#endif

QString EMPTYSTR=QString("");

LQMenu::LQMenu(QWidget * parent):QMenu(parent)
{

}

LQMenu::~LQMenu()
{

}

QAction* LQMenu::PopUp()
{
    return this->exec(QCursor::pos()-QPoint(this->width()/4,this->height()/3));
}

void LQMenu::leaveEvent(QEvent * event)
{
    Q_UNUSED(event);
    //printf("leave event");
    //先执行父类的event
    QMenu::leaveEvent(event);
    this->close();

}






MainUi::MainUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUi)
{
    ui->setupUi(this);
    //设置Widget为fixed,无法resize
    this->setFixedSize(this->size());
    //connect所有的按钮事件
    QObject::connect(ui->NewHostsPushButton,SIGNAL(clicked()),this,SLOT(NewHostsClicked()));
    QObject::connect(ui->OpenHostsPushButton,SIGNAL(clicked()),this,SLOT(OpenHostsClicked()));
    QObject::connect(ui->SearchDomainPushButton,SIGNAL(clicked()),this,SLOT(SearchDomainClicked()));
    QObject::connect(ui->AddDomainPushButton,SIGNAL(clicked()),this,SLOT(AddDomainClicked()));
    QObject::connect(ui->DelDomainPushButton,SIGNAL(clicked()),this,SLOT(DelDomainClicked()));
    QObject::connect(ui->SaveHostsPushButton,SIGNAL(clicked()),this,SLOT(SaveHostsClicked()));
    QObject::connect(ui->BackUpPushButton,SIGNAL(clicked()),this,SLOT(BackUpHostsClicked()));
    QObject::connect(ui->RestorePushButton,SIGNAL(clicked()),this,SLOT(RestoreHostsClicked()));
    QObject::connect(ui->CopyToSystemPushButton,SIGNAL(clicked()),this,SLOT(CopyToSystemClicked()));
    QObject::connect(ui->MKHostsPushButton,SIGNAL(clicked()),this,SLOT(MkHostsClicked()));

    //connect搜索文本事件
    QObject::connect(ui->SearchDomainLineEdit,SIGNAL(textEdited(const QString &)),this,SLOT(SearchDomainTextEdited(const QString &)));

    //树形控件单双击
    QObject::connect(ui->HostsTreeView,SIGNAL(clicked(const QModelIndex&)),this,SLOT(TreeItemClicked(const QModelIndex&)));
    QObject::connect(ui->HostsTreeView,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(TreeItemDoubleClicked(const QModelIndex&)));

    //设置域名搜索框的限制
    ui->SearchDomainLineEdit->setValidator(new DomainValidator(ui->SearchDomainLineEdit));

    //设置view list和模型关联
    ui->HostsTreeView->setModel(&TreeModel);
    ui->DomainlistView->setModel(&ListModel);
    //设置TreeView双击无法编辑
    ui->HostsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->DomainlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置listView可以选择多项
    ui->DomainlistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->DomainlistView->setLayoutMode(QListView::Batched);
    ui->DomainlistView->setBatchSize(15);

    //设置tree的右键菜单 设置上下文策略 如果参数填写的CustomContextMenu那么
    //会触发一个信号 customContextMenuRequested
    ui->HostsTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->HostsTreeView,SIGNAL(customContextMenuRequested(const QPoint&)),this,\
                                              SLOT(TreeViewRightClick(const QPoint &)));

    FreeCurrentUiRes();

    m_sys_host_full_path=QDir::rootPath();
    if(m_sys_host_full_path.length()==0)
    {
        MSGBOX("错误","无法获得系统路径");
        exit(0);
    }
    m_sys_host_full_path+=SYSTEM_HOSTS_SUFFIX;
}




MainUi::~MainUi()
{
    delete ui;
}



//对于tree右键的删除或者添加行为进行响应
bool MainUi::AddOrDel_IpOrGroup(RC_NODE_INFO* info)
{

    bool bret=false;
    if(info->op==OP_DEL)
    {
        QMessageBox::StandardButton ret=QMessageBox::information(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("你确定要移除这些数据吗?"),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Cancel);
        if(ret!=QMessageBox::Ok)
        {
            return true;
        }


    }

    if(info->op==OP_DEL)
    {

        //删除都是用的
        //删除所有的节点
        if(info->nodetype==NODETYPE_ROOT)
        {
            RemoveAllTreeItem();
            bret=true;
            //xml delete
            m_xml.RemoveAllIpNode();

        }
        else if(info->nodetype==NODETYPE_IP)
        {
            //删除IP节点
            if(info->index.isValid())
            {
                QString ipv4addr;
                QStandardItem* item=TreeModel.itemFromIndex(info->index);
                if(item)
                {
                    ipv4addr=item->data(TreeNodeDataRole).toString();
                }
                int prow=item->row();
                if(prow!=-1)
                {
                    item->parent()->removeRow(prow);
                }
                printf("rm ip node %s\n",ipv4addr.toStdString().c_str());
                bret=m_xml.DelIpNodeByIp(ipv4addr);
                if(!bret)
                {
                    printf("delete ipnode error\n");
                }
            }
        }
        else if(info->nodetype==NODETYPE_GROUP)
        {
            //del group
            if(info->index.isValid())
            {
                QString ipv4addr;
                QString groupname;

                QStandardItem* groupitem=TreeModel.itemFromIndex(info->index);
                QStandardItem* ipitem=NULL;
                if(groupitem)
                {
                    groupname=groupitem->data(TreeNodeDataRole).toString();
                    ipitem=groupitem->parent();
                     if(ipitem)
                     {
                         ipv4addr=ipitem->data(TreeNodeDataRole).toString();
                         ipitem->removeRow(groupitem->row());
                     }
                }
                printf("rm gr  node %s %s\n",groupname.toStdString().c_str(),ipv4addr.toStdString().c_str());
                bret=m_xml.DelGroupNode(groupname,ipv4addr);


            }
        }
        //如果是删除的话
        //模拟单击了root节点。重新更新数据。避免add出错
        ui->HostsTreeView->clicked(TreeModel.index(0,0));
        ListClear();
        //UpdateListView(NODETYPE_ROOT);
    }
    else if(info->op==OP_ADD)
    {
        if(!(info->desc.length()!=0&&info->data.length()!=0))
        {
            return bret;
        }

        if(info->nodetype==NODETYPE_ROOT)
        {
            //增加的是Ip节点
            bret=m_xml.AddNewIpNodeElement(info->data,info->desc);
            if(bret)
            {
               UpdateTreeView(NODETYPE_IP,info->data);
            }
        }
        else if(info->nodetype==NODETYPE_IP)
        {
            //增加的是分组节点
            if(info->index.isValid())
            {
                QStandardItem* ipitem=TreeModel.itemFromIndex(info->index);
                QString ipaddr;
                if(ipitem)
                {

                     ipaddr=ipitem->data(TreeNodeDataRole).toString();
                     bret=m_xml.AddGroupNodeElement(ipaddr,info->data,info->desc);
                     if(bret)
                     {
                         //xml add ok
                         TreeAddItem(NODETYPE_GROUP,info->desc,info->data,ipitem);
                     }

                }
            }

        }
    }
    else if(info->op==OP_MODIFY)
    {
        //QStandardItem* item=TreeModel.itemFromIndex(info->index);
        //编辑ip
        if(info->nodetype==NODETYPE_IP)
        {
            //update xml
            bret=m_xml.ModifyIpNodeElement(info->olddata,info->data,info->desc);
            if(bret)
            {
                QStandardItem* item=TreeModel.itemFromIndex(info->index);
                //刷新整个
                //QStandardItem* item=FindTreeItemByUSTR(info->nodetype,info->olddata);
                TreeModifyItem(item,(NODE_TYPE)info->nodetype,info->desc,info->data);
            }
            //update ui
        }
        else if(info->nodetype==NODETYPE_GROUP)
        {
            //编辑分组
            //update xml
            QStandardItem* groupItem=TreeModel.itemFromIndex(info->index);
            if(groupItem)
            {
                QStandardItem* IpItem=groupItem->parent();
                if(IpItem)
                {
                    QString ipstr=IpItem->data(TreeNodeDataRole).toString();
                    bret=m_xml.ModifyGroupNodeElement(ipstr,\
                                                      info->olddata,info->data,info->desc);
                    if(bret)
                    {
                        TreeModifyItem(groupItem,NODETYPE_GROUP,info->desc,info->data);
                    }
                }
            }




            //update ui
        }
        //模拟点击这个item
        m_selected_tree_item=QModelIndex();
        emit ui->HostsTreeView->clicked(info->index);
    }




    return bret;
}

bool MainUi::UpdateTreeView(int NodeType,QString& ipaddr)
{

    bool bret=false;
    do
    {
        if(NodeType==NODETYPE_ROOT)
        {

            QDomDocument dom=m_xml.GetDomDoc();
            if(dom.isNull())
            {
                LOGFATAL("UpdateTreeView dom Doc is NULL");
                break;
            }
            const QDomElement hosts_root=dom.firstChildElement(TAGNAME_ROOT);
            if(hosts_root.isNull())
            {
                LOGFATAL("UpdateTreeView hosts_root is NULL");
                break;
            }

            bret=UpdateTreeView(hosts_root,NodeType);
        }
        else if(NodeType==NODETYPE_IP)
        {
            //刷新IP节点下面的所有分组节点
            QDomElement ipNode=m_xml.FindIpNodeElement(ipaddr);
            if(ipNode.isNull())
            {
                break;
            }
            bret=UpdateTreeView(ipNode,NodeType);
        }
        else
        {
            break;
        }


    }while(false);
    return bret;


}

bool MainUi::UpdateTreeView(const QDomElement&Node,int NodeType)
{
    bool bret=false;
    do
    {

        if(Node.isNull())
        {
            LOGFATAL("UpdateTreeView Node=NULL");
            break;
        }

        QStandardItem* RootItem=FindTreeItemByUSTR(NODETYPE_ROOT);
        if(!RootItem)
        {
            LOGFATAL("UpdateTreeView RootItem=NULL");
            break;
        }

        if(NodeType==NODETYPE_ROOT)
        {
            //首先移除所有item
            RemoveAllTreeItem();

            QDomElement IpNode=Node.firstChildElement(TAGNAME_IPNODE);
            while(!IpNode.isNull())
            {

                UpdateTreeView(IpNode,NODETYPE_IP);
                IpNode=IpNode.nextSiblingElement(TAGNAME_IPNODE);
            }
        }
        else if(NodeType==NODETYPE_IP)
        {
            //首先移除ip节点下面的所有item 再重新添加
            QString attrip=Node.attribute(ATTRNAME_IP);
            QStandardItem*IpItem=FindTreeItemByUSTR(NODETYPE_IP,attrip);
            if(!IpItem)
            {
                //没有找到这个ipitem是新添加的。
                //break;
                QString ip=Node.attribute(ATTRNAME_IP);
                QString ip_desc=Node.attribute(ATTRNAME_DESC);
                if(ip.length()&&ip_desc.length())
                {
                   IpItem=TreeAddItem(MainUi::NODETYPE_IP,ip_desc,ip,RootItem);
                }
            }
            if(!IpItem)
            {
                LOGFATAL("UpdateTreeView can't find ipitem");
                break;
            }

            IpItem->removeRows(0,IpItem->rowCount());

            QDomElement GroupNode=Node.firstChildElement(TAGNAME_GROUP);
            while(!GroupNode.isNull())
            {
                QString group_name=GroupNode.attribute(ATTRNAME_NAME);
                QString group_desc=GroupNode.attribute(ATTRNAME_DESC);
                if(group_name.length()&&group_desc.length())
                {
                    TreeAddItem(MainUi::NODETYPE_GROUP,group_desc,group_name,IpItem);
                }
                GroupNode=GroupNode.nextSiblingElement(TAGNAME_GROUP);
            }
        }
        else
        {
            break;
        }
        bret=true;

    }while(false);
    return bret;
}

QStandardItem* MainUi::FindTreeItemByUSTR(int NodeType,QString& ipstr,QString& groupstr)
{
    QStandardItem* itemret=NULL;

    QStandardItem* rootItem=NULL;
    do
    {
        rootItem=TreeModel.item(0);
        if(NodeType==NODETYPE_ROOT)
        {
            itemret=rootItem;
            break;
        }
        if(!rootItem)
        {
            break;
        }
        if(ipstr.length()==0)
        {
            break;
        }
        QStandardItem* IpItem=NULL;
        for(int i=0;i<rootItem->rowCount();i++)
        {
            QStandardItem* it=rootItem->child(i);
            if(it)
            {
                if(it->data(TreeNodeDataRole).toString()==ipstr)
                {
                    IpItem=it;
                    break;
                }
            }
        }
        if(!IpItem)
        {
            break;
        }
        if(NodeType==NODETYPE_IP)
        {
            itemret=IpItem;
            break;
        }
        if(groupstr.length()==0)
        {
            break;
        }
        QStandardItem* groupItem=NULL;
        for(int i=0;i<IpItem->rowCount();i++)
        {
            QStandardItem* it=IpItem->child(i);
            if(it)
            {
                if(it->data(TreeNodeDataRole).toString()==groupstr)
                {
                    groupItem=it;
                    break;
                }
            }
        }
        if(NodeType==NODETYPE_GROUP)
        {
            itemret=groupItem;
            break;
        }

    }
    while(false);
    return itemret;
}

void MainUi::RemoveAllTreeItem()
{
    QStandardItem* rootItem=TreeModel.item(0);
    if(rootItem)
    {
        rootItem->removeRows(0,rootItem->rowCount());
    }
}

void MainUi::TreeViewRightClick(const QPoint & pos)
{

    QModelIndex ptIndex=ui->HostsTreeView->indexAt(pos);

    //如果当前没有打开或者新建，返回
    if(!m_xml.IsOpen())
    {
        return;
    }

    int ItemType=NODETYPE_ROOT;
    QStandardItem* curItem=NULL;
    if(ptIndex.isValid())
    {
        curItem=TreeModel.itemFromIndex(ptIndex);
        ItemType=curItem->data(TreeNodeTypeRole).toInt();
    }


    QString StrAdd;
    QString StrDel;
    QString StrModify;

    if(ItemType==NODETYPE_ROOT)
    {
        StrAdd=QString::fromLocal8Bit("添加IP节点");
        StrDel=QString::fromLocal8Bit("删除全部");
        //添加新的IP节点
        //删除所有节点
    }
    else if(ItemType==NODETYPE_IP)
    {
        //添加新的分组
        //删除当前IP
        StrAdd=QString::fromLocal8Bit("添加分组");
        StrDel=QString::fromLocal8Bit("删除该IP数据");
        StrModify=QString::fromLocal8Bit("编辑");
    }
    else if(ItemType==NODETYPE_GROUP)
    {
        //添加新的域名
        //删除当前分组
        StrAdd=QString::fromLocal8Bit("添加域名");
        StrDel=QString::fromLocal8Bit("删除该分组数据");
        StrModify=QString::fromLocal8Bit("编辑");

    }
    else
    {
        return;
    }

    LQMenu Menu(this);

    //分别添加菜单项
    QAction* acAdd=new QAction(StrAdd,&Menu);
    QAction* acDel=new QAction(StrDel,&Menu);

    Menu.addAction(acAdd);
    Menu.addAction(acDel);


    QAction* acModify=NULL;
    if(ItemType!=NODETYPE_ROOT)
    {
        acModify=new QAction(StrModify,&Menu);
        Menu.addAction(acModify);
    }




    printf("type=%d\n",ItemType);
    QAction * selectedAction=Menu.PopUp();
    if(!selectedAction)
    {
        return;
    }



    RC_NODE_INFO rcinfo;
    rcinfo.index=ptIndex;
    rcinfo.nodetype=ItemType;


    if(selectedAction==acAdd||selectedAction==acModify)
    {

        /*
         * 只有选中了某ip或者分组项的时候才会有modify选项
         * */

        if(selectedAction==acAdd)
            rcinfo.op=OP_ADD;
        else
        {
            rcinfo.op=OP_MODIFY;
            //填充原来的信息
            if(!curItem)
            {
                LOGFATAL("fatal error");
                //should never happen
                return;
            }
            rcinfo.olddata=curItem->data(TreeNodeDataRole).toString();
            rcinfo.olddesc=curItem->text();
        }



        //增加
        if(ItemType==NODETYPE_GROUP&&selectedAction==acAdd)
        {
            //分组的增加只要弹出XX对话框先设置选中这个index
            ui->HostsTreeView->setCurrentIndex(ptIndex);
            //直接调用
            //qt5
            {
                emit ui->HostsTreeView->clicked(ptIndex);
                emit ui->AddDomainPushButton->clicked();
            }

            // qt4
            {
               //AddDomainClicked();
            }
            //这个单独完成 返回
            return;
        }




        if (ItemType==NODETYPE_ROOT||(rcinfo.op==OP_MODIFY&&ItemType==NODETYPE_IP))
        {
            rcinfo.desc=QString::fromLocal8Bit("IP描述");
            rcinfo.data=QString::fromLocal8Bit("IP地址");
        }
        else
        {
            rcinfo.desc=QString::fromLocal8Bit("分组描述");
            rcinfo.data=QString::fromLocal8Bit("英文描述");
        }

        AddNodeDlg Dlg(&rcinfo,this);
        if(Dlg.exec()!=QDialog::Accepted)
        {
            return;
        }

    }
    else if(selectedAction==acDel)
    {
        //删除
        rcinfo.op=OP_DEL;
    }



    AddOrDel_IpOrGroup(&rcinfo);
}

//清理掉所有UI相关的资源。相当于从头再来
void MainUi::FreeCurrentUiRes()
{

    //添加域名的按钮为FALSE。只有选中了group以后才能变成可以添加的状态。
    ui->AddDomainPushButton->setEnabled(false);
    TreeModel.clear();
    ListModel.clear();
    ui->TreeItemInfolabel->setText(QString(""));
    m_xml.Close();
}





//根据当前的状态判断是否弹出对话框提示保存文件，true需要保存。false不需要保存
bool MainUi::BoxNeedSaveFile()
{
    bool needSave=false;
    do
    {
        if(!m_xml.IsOpen())
        {
            LOGDEBUG("BoxNeedSaveFile 文件尚未打开");
            break;
        }
        if(!m_xml.IsModified())
        {
            LOGDEBUG("BoxNeedSaveFile 文件没有被修改过");
            break;
        }
        if(QMessageBox::question(this,QString::fromLocal8Bit("保存"),\
                                 QString::fromLocal8Bit("文件被修改,是否需要保存?"),\
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::Yes)==QMessageBox::Yes)
        {
            needSave=true;
        }
    }while(false);
    return needSave;
}


//新建xml HOSTS
void MainUi::NewHostsClicked()
{
    OpenOrNewFile(false);
}

//打开或者new一个新的工程
bool MainUi::OpenOrNewFile(bool openfromfile,QString FileName)
{
    if(BoxNeedSaveFile())
    {
        //直接调用SaveHostsClicked();
        SaveHostsClicked();
    }
    //释放掉之前存在的一些资源
    FreeCurrentUiRes();


    bool bret=false;
    do
    {

        if(openfromfile)
        {
            if(FileName.length()==0)
            {
                FileName=ShowXmlFileDlg(false);
                if(!FileName.length())
                {
                     break;
                }
            }
            if(!m_xml.Open(FileName))
            {
                MSGBOX("错误","无法打开文件XML格式错误\n或者为空文件\n或者超过4MB大小");
                break;
            }

        }
        else
        {
            if(!m_xml.Create())
            {
                MSGBOX("错误","无法创建新的文件");
                break;
            }
        }

        if(!InitXmlUi())
        {
            MSGBOX("错误","非本软件需要的XML格式文件");

            break;
        }
        bret=true;
    }while(false);
    if(!bret)
    {
        FreeCurrentUiRes();
    }
















    return bret;
}


//打开xml HOSTS
void MainUi::OpenHostsClicked()
{
    OpenOrNewFile(true);
}


//搜索域名按钮
void MainUi::SearchDomainClicked()
{
    LOGDEBUG("SearchDomainClicked");
    if(Util::IsValidIpv4Str(ui->SearchDomainLineEdit->text()))
    {
        printf("valid ip");
    }
    else
    {
        printf("invalid ip");
    }

}

//增加域名
void MainUi::AddDomainClicked()
{
    LOGDEBUG("AddDomainClicked");
    QList<QString> DomainList;
    AddDomainDialog Dlg(&DomainList,this);
    if(QDialog::Accepted!=Dlg.exec())
    {
            return;
    }


    if(DomainList.empty())
    {
        return;

    }
    QStandardItem* groupItem=TreeModel.itemFromIndex(ui->HostsTreeView->currentIndex());
    if(!groupItem)
    {
        return;
    }

    if(groupItem->data(TreeNodeTypeRole).toInt()!=NODETYPE_GROUP)
    {
        LOGFATAL("fatal error invalid groupItem->data(TreeNodeTypeRole).toInt()!=NODETYPE_GROUP");
        return;
    }

    QString ip;
    QString groupname=groupItem->data(TreeNodeDataRole).toString();
    QStandardItem* IpItem=groupItem->parent();
    if(!IpItem)
    {
        LOGFATAL("fatal error invalid parent for groupItem");
        return ;
    }
    ip=IpItem->data(TreeNodeDataRole).toString();
    printf("%s %s",ip.toStdString().c_str(),groupname.toStdString().c_str());

    while(!DomainList.empty())
    {
        QString Domain=DomainList.front();
        DomainList.pop_front();
        //1 插入到 xml中。如果成功那么显示到UI上。否则不显示，比如已经存在了某个域名
        if(m_xml.AddNewDomainElement(ip,Domain,groupname))
        {
            ListAddDomain(ip,groupname,Domain);
        }
        printf("valid domain=%s\n",Domain.toStdString().c_str());
    }

    printf("AddDomainDialog return");
}
//删除选定域名
void MainUi::DelDomainClicked()
{
    LOGDEBUG("DelDomainClicked");
    QModelIndexList idel=ui->DomainlistView->selectionModel()->selectedIndexes();

    /*
     * 对这个索引从小到大排序。然后从大的往小的开始删除
     * 如果从小的开始删除。行号就变了。导致删除错误。
     * 使用qSort的要求是 对应的对象实现了<函数，可以用于比较。
     * 否则就只能用std::sort(a,b,compare_func)来实现
     * 由于QModelIndex已经实现了<比较符号。所以直接用qsort
     * 看源码，<的实现基本是通过比较row行号来实现的。row相等会比较其他的
     * */
    qSort(idel);



    while(!idel.empty())
    {
        QModelIndex id=idel.back();
        /*
         * 一个个开始删除
         * */

        //id.row()
        QStandardItem* item=ListModel.itemFromIndex(id);
        if(item)
        {
            QString domain=item->data(MainUi::ListItemDomainRole).toString();
            QString group=item->data(MainUi::ListItemGroupRole).toString();
            QString ip=item->data(MainUi::ListItemIpRole).toString();
            //在DOM节点里面删除
            m_xml.DelDomainNode(domain,ip,group);
            /*
            printf("%s %s %s\n",ip.toStdString().c_str(),\
                   group.toStdString().c_str(),\
                   domain.toStdString().c_str());
                   */
        }
        //界面上删除
        ListModel.removeRow(item->row());
        idel.pop_back();
    }

}



QString MainUi::ShowHostsFileDlg(bool save,QString defaultFilename)
{
    QString FileName;
    FileName.clear();
    QString DirName=QDir::currentPath();
    //末尾不包含斜杠
    QString XmlFilter="hosts file(*.hosts)";
    printf("CURDIR=%s\n",DirName.toStdString().c_str());
    DirName+=DIR_HOSTS_BACKUP;
    QDir Dir(DirName);
    if(!Dir.exists())
    {
        Dir.mkpath(DirName);
    }

    if(defaultFilename.length())
    {
        DirName+=defaultFilename;
    }
    else
        DirName+=DEFAULT_MAKE_HOSTS_NAME;

    if(save)
    {



        FileName=QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存hosts"),\
                                     DirName,\
                                     XmlFilter);
    }
    else
    {
        FileName=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开hosts"),\
                                              DirName,\
                                              XmlFilter);
    }
#ifdef _DEBUG
    printf("FileDlg FileName=%s\n",FileName.toStdString().c_str());
#endif
    return FileName;

}


QString MainUi::ShowXmlFileDlg(bool save)
{
    QString FileName;
    FileName.clear();
    QString DirName=QDir::currentPath();
    //末尾不包含斜杠
    QString XmlFilter="xml file(*.xml)";
    printf("CURDIR=%s\n",DirName.toStdString().c_str());
    DirName+=DIR_XMLFILE;
    QDir Dir(DirName);
    if(!Dir.exists())
    {
        Dir.mkpath(DirName);
    }

    if(save)
    {

        DirName+=DEFAULT_XML_FILENAME;
        FileName=QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存XML"),\
                                     DirName,\
                                     XmlFilter);
    }
    else
    {
        FileName=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开XML"),\
                                              DirName,\
                                              XmlFilter);
    }
#ifdef _DEBUG
    printf("FileDlg FileName=%s\n",FileName.toStdString().c_str());
#endif
    return FileName;
}






//保存xml hosts
void MainUi::SaveHostsClicked()
{
    LOGDEBUG("SaveHostsClicked");
    QString SaveFileName;
    do
    {
        if(!m_xml.IsOpen())
        {
            MSGBOX("没有活动的文件","请先新建或者打开一个文件");
            break;
        }
        if(!m_xml.IsModified())
        {
            break;
        }
        //如果是新建的文件。弹出保存对话框。得到文件名
        if(m_xml.IsNewFile())
        {
           //弹出对话框得到保存的文件名
           SaveFileName=ShowXmlFileDlg(true);
        }
        //如果SaveFileName长度为0，那么会使用默认打开的文件名字来保存
        bool bsave=m_xml.Save(SaveFileName);
        //已经被保存。设置没有修改
        m_xml.SetModified(false);
        if(!bsave)
        {
            MSGBOX("错误","保存XML文件失败");
        }
    }while(false);


}

bool MainUi::CopyHostsFile(QString& src,QString dest,bool removeifexist)
{
    bool bret=false;
    do
    {
        if(!src.length()||!dest.length())
        {
            MSGBOX("错误","文件名为空");
            break;
        }
#ifdef _DEBUG
        printf("copy %s to %s\n",src.toStdString().c_str(),\
               dest.toStdString().c_str());
#endif

        QFile srcFile(src);
        QFile destFile(dest);
        if(!srcFile.exists())
        {
            MSGBOX("错误","要拷贝的源文件不存在");
            break;
        }
        if(removeifexist)
        {
            if(destFile.exists())
            {
                if(!destFile.remove())
                {
                    MSGBOX("错误","无法删除源文件\n请使用管理员身份运行");
                    break;
                }
            }
        }
        bret=QFile::copy(src,dest);
        if(!bret)
        {
            MSGBOX("错误","无法拷贝文件");
            break;
        }
        bret=true;
    }while(false);
    return bret;
}



//备份当前系统hosts
void MainUi::BackUpHostsClicked()
{
    LOGDEBUG("BackUpHostsClicked");
    QString dest=ShowHostsFileDlg(true,"hosts_"+Util::timestr()+".hosts");
    if(dest.length())
    {
        CopyHostsFile(m_sys_host_full_path,dest,true);
    }
}

//从备份的hosts恢复到系统
void MainUi::RestoreHostsClicked()
{
    LOGDEBUG("RestoreHostsClicked");
    CopyToSystemClicked();

}






//生成hosts
void MainUi::MkHostsClicked()
{
    LOGDEBUG("MkHostsClicked");

    if(!m_xml.IsOpen())
    {
        return;
    }

    QString FileName=ShowHostsFileDlg(true);
    if(FileName.length())
        m_xml.ToHosts(FileName);
}

//将当前生成的hosts放到系统
void MainUi::CopyToSystemClicked()
{
    LOGDEBUG("CopyToSystemClicked");
    QString src=ShowHostsFileDlg(false);
    if(src.length())
    {
        if(CopyHostsFile(src,m_sys_host_full_path,true))
        {
            MSGBOX("成功","已经拷贝到系统");
        }
    }
}

//搜索框编辑字符
void MainUi::SearchDomainTextEdited(const QString& text)
{
    Q_UNUSED(text);
    /*
    LOGDEBUG("SearchDomainTextEdited");
    if(!Util::IsValidDomainStr(text))
    {
        LOGDEBUG("Invalid Doamin Str");
    }
    else
    {

    }*/
}

bool MainUi::UpdateListView(int NodeType,QString& ipaddr,QString& groupname)
{


    if(NodeType==MainUi::NODETYPE_DOMAIN)
    {
        return false;
    }


    QDomElement Node;
    do
    {
        Node=m_xml.GetDomDoc().firstChildElement(TAGNAME_ROOT);
        if(NodeType==MainUi::NODETYPE_ROOT)
        {
            break;
        }
        if(ipaddr.length()==0)
        {
            return false;
        }
        Node=Node.firstChildElement(TAGNAME_IPNODE);
        while(!Node.isNull())
        {
            if(Node.attribute(ATTRNAME_IP)==ipaddr)
            {
                  break;
            }
            Node=Node.nextSiblingElement(TAGNAME_IPNODE);
        }
       //没有找到这个IP节点
        if(Node.isNull())
        {
            break;
        }
        if(NodeType==MainUi::NODETYPE_IP)
        {
            break;
        }

        if(groupname.length()!=0)
        {
            Node=Node.firstChildElement(TAGNAME_GROUP);
            while(!Node.isNull())
            {
                if(Node.attribute(ATTRNAME_NAME)==groupname)
                {
                    break;
                }
                Node=Node.nextSiblingElement(TAGNAME_GROUP);
            }
        }
        //没有找到这个group节点
        if(Node.isNull())
        {
            break;
        }
        if(NodeType==MainUi::NODETYPE_GROUP)
        {
            break;
        }
        return false;
    }
    while(false);
    if(Node.isNull())
    {
        return false;
    }
    return UpdateListView(Node,NodeType);
}

void MainUi::ListClear()
{
    ListModel.clear();
}

void MainUi::ListAddDomain(QString& ipaddr,QString& groupname,QString& domain)
{

    int len=ipaddr.length();
    if(len<7||len>15)
    {
        return;
    }
    len=groupname.length();
    if(len==0||len>MAX_DESC_LEN)
    {
        return;
    }
    len=domain.length();
    if(len==0||len>MAX_DOMAIN_LEN)
    {
        return;
    }

    QStandardItem* Item=new QStandardItem(domain);
    Item->setData(ipaddr,MainUi::ListItemIpRole);
    Item->setData(groupname,MainUi::ListItemGroupRole);
    Item->setData(domain,MainUi::ListItemDomainRole);
    ListModel.appendRow(Item);
}

bool MainUi::UpdateListView(QDomElement&Node,int NodeType)
{
    if(Node.isNull())
    {
        return false;
    }

    QString ChildTag;
    int     ChildType;

    if(NodeType==MainUi::NODETYPE_ROOT)
    {
        ChildTag=TAGNAME_IPNODE;
        ChildType=MainUi::NODETYPE_IP;
    }
    else if(NodeType==MainUi::NODETYPE_IP)
    {
        ChildTag=TAGNAME_GROUP;
        ChildType=MainUi::NODETYPE_GROUP;
    }
    else if(NodeType==MainUi::NODETYPE_GROUP)
    {
        ChildTag=TAGNAME_DOMAIN;
        ChildType=MainUi::NODETYPE_DOMAIN;
    }
    else if(NodeType==MainUi::NODETYPE_DOMAIN)
    {
        QDomNode DNode=Node.firstChild();
        if(DNode.isNull())
        {
            return false;
        }
        if(!DNode.isText())
        {
            return false;
        }
        QDomText DText=DNode.toText();
        QString domain=DText.data();
        if(domain.length()>MAX_DOMAIN_LEN||domain.length()==0)
        {
          return false;
        }
        QDomElement ipNode=Node.parentNode().parentNode().toElement();
        QDomElement GroupNode=Node.parentNode().toElement();

        if(ipNode.isNull()||GroupNode.isNull())
        {
            return false;
        }
        QString ip=ipNode.attribute(ATTRNAME_IP);
        QString groupname=GroupNode.attribute(ATTRNAME_NAME);
        if(ip.length()==0||groupname.length()==0)
        {
            return false;
        }


        ListAddDomain(ip,groupname,domain);

        return true;
    }

    QDomElement ChildNode=Node.firstChildElement(ChildTag);
    //bool bret=true;
    while(!ChildNode.isNull())
    {
        UpdateListView(ChildNode,ChildType);
        /*
        if(!ok)
        {
            bret=false;
        }*/
        ChildNode=ChildNode.nextSiblingElement(ChildTag);
    }
    return true;
}


void MainUi::TreeItemClicked(const QModelIndex & index)
{

    if(m_selected_tree_item.isValid()&&m_selected_tree_item==index)
    {
        //单击的是同一个项目。避免重复刷新
#ifdef _DEBUG
        printf("same id \n");
#endif
        return ;
    }
    else
    {
        //清理掉当前的列表。重新刷新
        ListClear();
        m_selected_tree_item=index;
    }




    //单击触发右侧列表刷新
    QStandardItem* Item=TreeModel.itemFromIndex(index);
    if(!Item)
    {
        return;
    }

    int type=Item->data(MainUi::TreeNodeTypeRole).toInt();
    if(type==MainUi::NODETYPE_ROOT)
    {
        UpdateListView(type);
    }
    else if(type==MainUi::NODETYPE_IP)
    {
        QString ipaddr=Item->data(MainUi::TreeNodeDataRole).toString();
        UpdateListView(type,ipaddr);
    }
    else if(type==MainUi::NODETYPE_GROUP)
    {


        QStandardItem* parent=Item->parent();

        QString ipaddr=parent->data(MainUi::TreeNodeDataRole).toString();
        QString groupname=Item->data(MainUi::TreeNodeDataRole).toString();
        UpdateListView(type,\
                       ipaddr,\
                       groupname);
    }
    else
    {
        return;
    }

    QString info=Item->data(MainUi::TreeNodeDataRole).toString();
    ui->TreeItemInfolabel->setText(info);



    if(type==MainUi::NODETYPE_GROUP)
    {
        /*
         * 设置添加按钮可用
         * */
        ui->AddDomainPushButton->setEnabled(true);
    }
    else
    {
        /*
         * 设置添加按钮不可用
         * */
        ui->AddDomainPushButton->setEnabled(false);
    }


}

void MainUi::TreeItemDoubleClicked(const QModelIndex & index)
{
    //由于双击一定会出发单击事件。所有双全部不处理。改成右键弹出菜单处理。
   // printf("tree item index dbclicked");
    Q_UNUSED(index);
}
QStandardItem* MainUi::TreeModifyItem(QStandardItem* item,NODE_TYPE type,QString& DisplayText,QString & data)
{
    if(item)
    {
        item->setText(DisplayText);
        item->setData(data,MainUi::TreeNodeDataRole);
        item->setData(type,MainUi::TreeNodeTypeRole);
    }
    return item;
}

QStandardItem* MainUi::TreeAddItem(MainUi::NODE_TYPE type,QString& DisplayText,QString & data,QStandardItem* parent)
{
    int len=DisplayText.length();
    if(len==0||len>MAX_DESC_LEN)
        return NULL;

    len=data.length();
    if(len==0||len>MAX_ATTRNAME_LEN)
        return NULL;

    QStandardItem* Item=new QStandardItem();
    parent->appendRow(Item);

    TreeModifyItem(Item,type,DisplayText,data);
    return Item;
}

//将当前XML DOC里面的内容绘制到UI上
bool MainUi::InitXmlUi()
{

    bool bret=false;
    do
    {
       QDomDocument dom=m_xml.GetDomDoc();
       if(dom.isNull())
       {
           LOGFATAL("dom Doc is NULL");
            break;
       }
       QString title=m_xml.GetFileName();
       int fileNameIndex=title.lastIndexOf(QRegExp("[/\\\\]"));
       if(fileNameIndex!=-1)
       {
            title=title.right(title.length()-fileNameIndex-1);
       }

       TreeModel.setHorizontalHeaderItem(0,new QStandardItem(title));
       const QDomElement hosts_root=dom.firstChildElement(TAGNAME_ROOT);
       if(hosts_root.isNull())
       {
           break;
       }
       QString roottagname=hosts_root.tagName();
       QStandardItem* RootItem=TreeAddItem(MainUi::NODETYPE_ROOT,roottagname,roottagname,TreeModel.invisibleRootItem());
       if(!RootItem)
       {
           break;
       }


      bret=UpdateTreeView(NODETYPE_ROOT);

    }while(false);
    return bret;
}



