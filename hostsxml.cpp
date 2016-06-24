#include "hostsxml.h"
#include "util.h"

#ifdef _WIN32
#include <Winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

HostsXml::HostsXml()
{
   Close();
}

HostsXml::~HostsXml()
{
    Close();
}


QString HostsXml::GetFileName()
{

 if(m_filename.length())
 {
     return m_filename;
 }
 else
 {
     return QString("untitled");
 }

}

QDomElement HostsXml::FindDomainElement(QDomElement& ipNode,const QString& GroupName,const QString& Domain)
{
    QDomElement DomainNode;
    QDomElement GroupNode;

    GroupNode=FindGroupNodeElement(ipNode,GroupName);
    if(!GroupNode.isNull())
    {
        DomainNode=FindDomainElement(GroupNode,Domain);
    }
    return DomainNode;
}

QDomElement HostsXml::FindDomainElement(QDomElement& GroupNode,const QString& Domain)
{
    QDomElement DomainNode;
    QDomElement CurDomainNode=GroupNode.firstChildElement(TAGNAME_DOMAIN);

    if(Domain.length()==0)
    {
        LOGFATAL("FindDomainElement Domain.length()==0");
        return DomainNode;
    }

    while(!CurDomainNode.isNull())
    {
        if(CurDomainNode.text()==Domain)
        {
            DomainNode=CurDomainNode;
            break;
        }
        CurDomainNode=CurDomainNode.nextSiblingElement(TAGNAME_DOMAIN);
    }

    if(DomainNode.isNull())
    {
        LOGFATAL("FindDomainElement can't find Doamin");
        LOGFATAL(Domain.toStdString().c_str());
    }

    return DomainNode;
}

QDomElement HostsXml::FindGroupNodeElement(QDomElement& IpNode,const QString& GroupName)
{
    QDomElement GroupNode;
    QDomElement CurGroupNode=IpNode.firstChildElement(TAGNAME_GROUP);
    if(GroupName.length()==0)
    {
        return GroupNode;
    }

    while(!CurGroupNode.isNull())
    {
        if(CurGroupNode.attribute(ATTRNAME_NAME)==GroupName)
        {
            GroupNode=CurGroupNode;
            break;
        }
        CurGroupNode=CurGroupNode.nextSiblingElement(TAGNAME_GROUP);
    }

    if(CurGroupNode.isNull())
    {
        LOGFATAL("FindGroupNodeElement can't find groupNode");
        LOGFATAL(GroupName.toStdString().c_str());
    }

    return GroupNode;
}


QDomElement HostsXml::FindIpNodeElement(const QString& ipv4addr)
{
    QDomElement IpNode;
    do
    {
        if(m_root.isNull()||ipv4addr.length()==0)
        {
            LOGFATAL("FindIpNodeElement m_root.isNull");
            break;
        }
        QDomElement CurIpNode=m_root.firstChildElement(TAGNAME_IPNODE);
        while(!CurIpNode.isNull())
        {
            if(CurIpNode.attribute(ATTRNAME_IP)==ipv4addr)
            {
                IpNode=CurIpNode;
                break;
            }
            CurIpNode=CurIpNode.nextSiblingElement(TAGNAME_IPNODE);
        }
    }while(false);

    if(IpNode.isNull())
    {
        LOGFATAL("FindIpNodeElement can't find IpNode");
        LOGFATAL(ipv4addr.toStdString().c_str());
    }

    return IpNode;
}

QDomElement HostsXml::FindGroupNodeElement(const QString& ipv4addr,const QString& GroupName)
{
    QDomElement GroupNode;
    QDomElement IpNode=FindIpNodeElement(ipv4addr);
    if(!IpNode.isNull())
    {
        GroupNode=FindGroupNodeElement(IpNode,GroupName);
    }
    return GroupNode;
}

QDomElement HostsXml::FindDomainElement(const QString& ipv4addr,const QString& GroupName,const QString& Domain)
{
    QDomElement DomainNode;
    QDomElement GroupNode=FindGroupNodeElement(ipv4addr,GroupName);
    if(!GroupNode.isNull())
    {
        DomainNode=FindDomainElement(GroupNode,Domain);
    }
    return DomainNode;
}


QDomElement HostsXml::IAddNewIpNodeElement(QString& ipv4addr,QString& desc)
{
    QDomElement IpNode;
    do
    {
        if(!FindIpNodeElement(ipv4addr).isNull())
        {
            break;
        }
        if(ipv4addr.length()<7||ipv4addr.length()>15||desc.length()==0)
        {
            break;
        }
        //创建第一个IP节点 <ipnode></ipnode>
        IpNode=m_doc.createElement(TAGNAME_IPNODE);
        //设置属性<ipnode ip=ipv4addr desc=desc></ipnode>
        IpNode.setAttribute(ATTRNAME_DESC,desc);
        IpNode.setAttribute(ATTRNAME_IP,ipv4addr);
        QString name=QString::fromLocal8Bit(GROUP_DEFAULT_NAME);
        QString desc=QString::fromLocal8Bit(GROUP_DEFAULT_DESC);
        IAddGroupNodeElement(IpNode,name,desc);
        m_root.appendChild(IpNode);
        m_modified=true;
    }while(false);
    return IpNode;
}

QDomElement HostsXml::IAddGroupNodeElement(QString& ipv4addr,QString& GroupName,QString& desc)
{
    QDomElement GroupNode;
    QDomElement IpNode=FindIpNodeElement(ipv4addr);
    if(!IpNode.isNull())
    {
        GroupNode=IAddGroupNodeElement(IpNode,GroupName,desc);
    }
    return GroupNode;
}

QDomElement HostsXml::IAddGroupNodeElement(QDomElement& IpNode,QString& GroupName,QString& desc)
{
    QDomElement GroupNode;

    if(GroupName.length()==0||desc.length()==0)
    {
        return GroupNode;
    }
    if(!FindGroupNodeElement(IpNode,GroupName).isNull())
    {
        return GroupNode;
    }
    GroupNode=m_doc.createElement(TAGNAME_GROUP);
    GroupNode.setAttribute(ATTRNAME_DESC,desc);
    GroupNode.setAttribute(ATTRNAME_NAME,GroupName);
    IpNode.appendChild(GroupNode);
    m_modified=true;
    return GroupNode;

}


QDomElement HostsXml::IAddNewDomainElement(QString& ipv4addr,QString& Domain,QString& groupname)
{
    QDomElement DomainNode;
    QDomElement GroupNode=FindGroupNodeElement(ipv4addr,groupname);
    if(!GroupNode.isNull())
    {
        DomainNode=IAddNewDomainElement(GroupNode,Domain);
    }
    return DomainNode;
}

QDomElement HostsXml::IAddNewDomainElement(QDomElement& IpNode,QString& Domain,QString& groupname)
{
    QDomElement DomainNode;
    QDomElement GroupNode=FindGroupNodeElement(IpNode,groupname);
    if(!GroupNode.isNull())
    {
        DomainNode=IAddNewDomainElement(GroupNode,Domain);
    }
    return DomainNode;
}

QDomElement HostsXml::IAddNewDomainElement(QDomElement& GroupNode,QString& Domain)
{
    QDomElement DomainNode;
    if(!FindDomainElement(GroupNode,Domain).isNull())
    {
        return DomainNode;
    }
    int len=Domain.length();
    if(len==0||len>MAX_DOMAIN_LEN)
    {
        return DomainNode;
    }

    DomainNode=m_doc.createElement(TAGNAME_DOMAIN);
    DomainNode.appendChild(m_doc.createTextNode(Domain));
    GroupNode.appendChild(DomainNode);
    m_modified=true;
    return DomainNode;
}

/*
 * 添加修改删除IP节点
 * */
bool HostsXml::AddNewIpNodeElement(QString& ipv4addr,QString& desc)
{
    QDomElement IpNode=IAddNewIpNodeElement(ipv4addr,desc);
    if(IpNode.isNull())
    {
        return false;
    }
    return true;
}


void HostsXml::RemoveAllIpNode()
{
    QDomElement root=m_doc.firstChildElement(TAGNAME_ROOT);
    if(!root.isNull())
    {
        QDomNodeList clist=root.childNodes();
        for(int i=0;i<clist.count();i++)
            root.removeChild(clist.at(i));
    }
}

bool HostsXml::ModifyIpNodeElement(QString& oldipv4addr,QString& newipv4addr,QString& newdesc)
{
    QDomElement IpNode=FindIpNodeElement(newipv4addr);
    if(!IpNode.isNull())
    {
        return false;
    }
    IpNode=FindIpNodeElement(oldipv4addr);
    if(IpNode.isNull())
    {
        return false;
    }

    IpNode.setAttribute(ATTRNAME_IP,newipv4addr);
    IpNode.setAttribute(ATTRNAME_DESC,newdesc);
    m_modified=true;
    return true;
}

bool HostsXml::DelIpNodeByIp(QString& ipaddr)
{
    QDomElement IpNode=FindIpNodeElement(ipaddr);
    if(IpNode.isNull())
    {
        LOGFATAL("DelIpNodeByIp ipNode Is Null");
        return false;
    }
    if(IpNode.parentNode().isNull())
    {
        LOGFATAL("DelIpNodeByIp IpNode.parentNode().isNull");
        return false;
    }
    IpNode.parentNode().removeChild(IpNode);
    m_modified=true;
    return true;
}

/*
 *添加修改删除组节点
 * */
bool HostsXml::AddGroupNodeElement(QString& ipv4addr,QString& GroupName,QString& desc)
{
    QDomElement GroupNode=IAddGroupNodeElement(ipv4addr,GroupName,desc);
    if(GroupNode.isNull())
    {
        return false;
    }
    return true;
}

bool HostsXml::ModifyGroupNodeElement(QString& ipv4addr,QString& OldGroupName,QString& NewGroupName,QString& newdesc)
{
    QDomElement GroupNode=FindGroupNodeElement(ipv4addr,NewGroupName);
    if(!GroupNode.isNull())
    {
        return false;
    }
    GroupNode=FindGroupNodeElement(ipv4addr,OldGroupName);
    if(GroupNode.isNull())
    {
        return false;
    }
    if(NewGroupName.length()==0||newdesc.length()==0)
    {
        return false;
    }

    GroupNode.setAttribute(ATTRNAME_NAME,NewGroupName);
    GroupNode.setAttribute(ATTRNAME_DESC,newdesc);

    m_modified=true;
    return true;

}
bool HostsXml::DelGroupNode(QString& GroupName,QString& ipv4addr)
{
    QDomElement GroupNode=FindGroupNodeElement(ipv4addr,GroupName);
    if(GroupNode.isNull())
    {
        return false;
    }
    if(GroupNode.parentNode().isNull())
    {
       return false;
    }
    GroupNode.parentNode().removeChild(GroupNode);
    m_modified=true;
    return true;
}

/*
 * 添加修改删除域名节点
 * */
bool HostsXml::AddNewDomainElement(QString& ipv4addr,QString& Domain,QString& groupname)
{
    QDomElement DomainNode=IAddNewDomainElement(ipv4addr,Domain,groupname);
    if(DomainNode.isNull())
    {
        return false;
    }
    return true;
}

bool HostsXml::ModifyDomainElement(QString& ipv4addr,QString& groupname,QString& OldDomain,QString& NewDomain)
{
    QDomElement DomainNode=FindDomainElement(ipv4addr,groupname,NewDomain);
    if(!DomainNode.isNull())
    {
        return false;
    }
    DomainNode=FindDomainElement(ipv4addr,groupname,OldDomain);
    if(DomainNode.isNull()||!DomainNode.hasChildNodes())
    {
        return false;
    }
    DomainNode.firstChild().setNodeValue(NewDomain);
    m_modified=true;
    return true;
}

bool HostsXml::DelDomainNode(QString& domain,QString& ipv4addr,QString& GroupName)
{
    QDomElement DomainNode=FindDomainElement(ipv4addr,GroupName,domain);
    if(DomainNode.isNull())
    {
        return false;
    }
    if(DomainNode.parentNode().isNull())
    {
        return false;
    }
    DomainNode.parentNode().removeChild(DomainNode);
    m_modified=true;
    return true;
}

void HostsXml::AddComputerHostName()
{

    char CHostName[MAX_DOMAIN_LEN];
    memset(CHostName,0,MAX_DOMAIN_LEN);
#ifdef _WIN32
#else
    if(!gethostname(CHostName,MAX_DOMAIN_LEN-1))
    {
        //检查是否是有效的主机名
        QString QName=QString(CHostName);
        if(!Util::IsValidDomainStr(QName))
        {
            return;
        }

        QString IpStr=QString(LOCAL_IP);
        QString GroupStr=QString(GROUP_DEFAULT_NAME);
        QDomElement QDomainElement=FindDomainElement(IpStr,GroupStr,QName);
        if(!QDomainElement.isNull())
        {
            return;
        }
        AddNewDomainElement(IpStr,QName,GroupStr);
    }
#endif
}

bool HostsXml::Open(QString& fileName)
{
    Close();
    m_modified=false;
    m_isnewfile=false;
    m_isopen=true;
    m_filename=fileName;
    bool bret=false;
    do
    {
        QFile File(fileName);
        if(!File.open(QIODevice::ReadWrite))
        {
            LOGFATAL("can't open file to read");
            break;
        }
        if(File.size()>=MAX_XML_FILESIZE)
        {
            LOGFATAL("File.size()>=MAX_XML_FILESIZE");
            break;
        }
        if(!m_doc.setContent(&File))
        {
            LOGFATAL("can't parser xml file");
            break;
        }
        m_root=m_doc.firstChildElement(TAGNAME_ROOT);
        if(m_root.isNull())
        {
            LOGFATAL("invalid xml file,without TAGNAME_ROOT");
            break;
        }
        File.close();
        bret=true;
    }while(false);
    if(!bret)
    {
        Close();
    }
    else
    {
        AddComputerHostName();
    }


    return bret;
}

bool HostsXml::Create()
{
    Close();
    //创建根元素 <hosts></hosts>
    m_root=m_doc.createElement(TAGNAME_ROOT);
    m_doc.appendChild(m_root);
    QString ipaddr=QString(LOCAL_IP);
    QString ipdesc=QString::fromLocal8Bit("本机地址");
    QDomElement IpNode=IAddNewIpNodeElement(ipaddr,ipdesc);
    QString gname=QString::fromLocal8Bit(GROUP_DEFAULT_NAME);
    QString domain=QString("localhost");
    IAddNewDomainElement(IpNode,domain,gname);
    domain=QString("ubuntu");
    IAddNewDomainElement(IpNode,domain,gname);


    AddComputerHostName();

    m_isopen=true;
    m_modified=true;
    m_isnewfile=true;
    return true;
}

bool HostsXml::Save(QString& fileName)
{
    QString SaveName;
    if(fileName.length()!=0)
    {
        SaveName=fileName;
    }
    else if(m_filename.length()!=0)
    {
        SaveName=m_filename;
    }
    else if(m_doc.isNull())
    {
        LOGFATAL("DOM doc is=NULL");
        return false;
    }
    else
    {
        LOGFATAL("savefile fileName=NULL");
        return false;
    }

    QFile File(SaveName);
    if(File.exists())
    {
        File.remove();
    }

    do
    {
        if(! File.open(QIODevice::ReadWrite))
        {
            LOGFATAL("can't open file for save");
            break;
        }
        QTextStream stream(&File);
        if(stream.status()!=QTextStream::Ok)
        {
            LOGFATAL("invalid stream to save");
            break;
        }
        m_doc.save(stream,2);


    }while(false);




    return true;
}




void HostsXml::Close()
{
    m_isopen=false;
    m_modified=false;
    m_isnewfile=false;
    m_filename.clear();
    m_root.clear();
    m_doc.clear();
}

bool HostsXml::IsOpen()
{
    return m_isopen;
}

bool HostsXml::IsModified()
{
    return m_modified;
}

qint64 HostsXml::HostsWriteOneLine(QFile& file,const QString& str,bool iscomment)
{
    std::string stdstr=str.toStdString();
    return HostsWriteOneLine(file,stdstr.c_str(),stdstr.length(),iscomment);
}

qint64 HostsXml::HostsWriteOneLine(QFile& file, const char*  str,qint64 len, bool iscomment)
{
    qint64 wrlen=-1;
    qint64 wl=0;
    do
    {
        if(!(file.isWritable()&&len<128))
        {
            break;
        }
        if(iscomment)
        {
            wl=file.write("#",1);
            if(-1==wl)
            {
                break;
            }
            wrlen+=wl;
        }

        if(len>0)
        {
            wl=file.write(str,len);
            if(-1==wl)
            {
                break;
            }
            wrlen+=wl;
        }



#ifdef _WIN32
#define CRLF "\r\n"
#else
#define CRLF "\n"
#endif
       wl=file.write(CRLF,sizeof(CRLF)-1);
       if(-1==wl)
       {
           break;
       }
       wrlen+=wl;
    }while(false);
    return wrlen;
}



bool HostsXml::ToHosts(QString fileName)
{
    bool bret=false;

    do
    {
        if(!IsOpen())
        {
            LOGFATAL("not open yet");
            break;
        }

        if(!fileName.length())
        {
            LOGFATAL("invalid hosts name");
            break;
        }

        QFile file(fileName);
        if(file.exists())
        {
            file.remove();
        }
        if(!file.open(QIODevice::ReadWrite))
        {
            LOGFATAL("can't openfile for write");
            break;
        }

        QDomElement root=m_doc.firstChildElement(TAGNAME_ROOT);
        if(root.isNull())
        {
            LOGFATAL("root is null");
            break;
            break;
        }

        QDomElement ipNode=root.firstChildElement(TAGNAME_IPNODE);

        HostsWriteOneLine(file,QString::fromLocal8Bit("生成日期")+Util::timestr(),true);
        while(!ipNode.isNull())
        {
            QString ipdesc=ipNode.attribute(ATTRNAME_DESC);
            QString ipaddr=ipNode.attribute(ATTRNAME_IP);

            if(ipdesc.length()&&ipaddr.length())
            {

                printf("%s %s\n",ipdesc.toStdString().c_str(),ipaddr.toStdString().c_str());
                HostsWriteOneLine(file,QString::fromLocal8Bit("IP信息")+ipaddr+" "+ipdesc,true);
                QDomElement group=ipNode.firstChildElement(TAGNAME_GROUP);

                while(!group.isNull())
                {

                    QString groupdesc=group.attribute(ATTRNAME_DESC);
                    QString groupname=group.attribute(ATTRNAME_NAME);

                    if(groupdesc.length()&&groupname.length())
                    {
                        // printf("%s %s\n",groupdesc.toStdString().c_str(),groupname.toStdString().c_str());
                        HostsWriteOneLine(file,QString::fromLocal8Bit("分组信息")+groupname+" "+groupdesc,true);
                        QDomElement domainNode=group.firstChildElement(TAGNAME_DOMAIN);
                        while(!domainNode.isNull())
                        {

                           if(domainNode.firstChild().isText())
                           {
                               QString domain=domainNode.firstChild().toText().data();
                               HostsWriteOneLine(file,ipaddr+" "+domain);
                           }


                            domainNode=domainNode.nextSiblingElement(TAGNAME_DOMAIN);
                        }
                        HostsWriteOneLine(file,NULL,0);
                    }
                    group=group.nextSiblingElement(TAGNAME_GROUP);
                }

                HostsWriteOneLine(file,NULL,0);
                HostsWriteOneLine(file,NULL,0);
                HostsWriteOneLine(file,NULL,0);
                HostsWriteOneLine(file,NULL,0);

            }




            ipNode=ipNode.nextSiblingElement(TAGNAME_IPNODE);
        }

        file.close();




    }while(false);


    return bret;
}


