#ifndef HOSTSXML
#define HOSTSXML
#include <QtXml/QtXml>
#include <QtCore/QFile>
#include "logger.h"
#include <QtXml/QDomElement>

/*
 * 由于是用DOM来解析，全部放在内存当中。
 * 所以适用于XML小文件。不要超过 4M
 */
#ifndef MAX_XML_FILESIZE
#define MAX_XML_FILESIZE 4*1024*1024
#endif

//域名的最大长度
#ifndef MAX_DOMAIN_LEN
#ifdef _DEBUG
#define MAX_DOMAIN_LEN 64
#else
#define MAX_DOMAIN_LEN 64
#endif
#endif

#ifndef MAX_ATTRNAME_LEN
#define MAX_ATTRNAME_LEN 16
#define MAX_DESC_LEN     16
#endif


#ifndef HOSTS_XML_DEF_NAME
#define HOSTS_XML_DEF_NAME

#define TAGNAME_ROOT    "hosts"
#define TAGNAME_IPNODE  "ipnode"
#define TAGNAME_GROUP   "group"
#define TAGNAME_DOMAIN  "domain"
#define ATTRNAME_IP     "ip"
#define ATTRNAME_NAME   "name"
#define ATTRNAME_DESC   "desc"

#define GROUP_DEFAULT_NAME "default"
#define GROUP_DEFAULT_DESC "默认分组"


#endif


#ifndef LOCAL_IP
#define LOCAL_IP "127.0.0.1"
#endif



class HostsXml
{
public:
    HostsXml();
    ~HostsXml();
    bool Open(QString& fileName);
    bool Create();
    void Close();
    bool IsOpen();
    bool IsModified();
    bool IsNewFile(){return m_isnewfile;}
    void SetModified(bool modified){m_modified=modified;}
    const QDomDocument& GetDomDoc(){return m_doc;}
    bool Save(QString& fileName);
    QString GetFileName();
    /*
     * 添加修改删除IP节点
     * */
    bool AddNewIpNodeElement(QString& ipv4addr,QString& desc);
    bool ModifyIpNodeElement(QString& oldipv4addr,QString& newipv4addr,QString& newdesc);
    bool DelIpNodeByIp(QString& ipaddr);

    /*
     *添加修改删除组节点
     * */
    bool AddGroupNodeElement(QString& ipv4addr,QString& GroupName,QString& desc);
    bool ModifyGroupNodeElement(QString& ipv4addr,QString& OldGroupName,QString& NewGroupName,QString& newdesc);
    bool DelGroupNode(QString& GroupName,QString& ipv4addr);
    /*
     * 添加修改删除域名节点
     * */
    bool AddNewDomainElement(QString& ipv4addr,QString& Domain,QString& groupname);
    bool ModifyDomainElement(QString& ipv4addr,QString& groupname,QString& OldDomain,QString& NewDoamin);
    bool DelDomainNode(QString& domain,QString& ipv4addr,QString& GroupName);

    void RemoveAllIpNode();



    QDomElement FindIpNodeElement(const QString& ipv4addr);

    QDomElement FindGroupNodeElement(const QString& ipv4addr,const QString& GroupName);
    QDomElement FindGroupNodeElement(QDomElement& ipNode,const QString& GroupName);


    QDomElement FindDomainElement(const QString& ipv4addr,const QString& GroupName,const QString& Domain);
    QDomElement FindDomainElement(QDomElement& ipNode,const QString& GroupName,const QString& Domain);
    QDomElement FindDomainElement(QDomElement& GroupNode,const QString& Domain);

    QDomElement IAddNewIpNodeElement(QString& ipv4addr,QString& desc);
    QDomElement IAddGroupNodeElement(QString& ipv4addr,QString& GroupName,QString& desc);
    QDomElement IAddGroupNodeElement(QDomElement& IpNode,QString& GroupName,QString& desc);

    QDomElement IAddNewDomainElement(QString& ipv4addr,QString& Domain,QString& groupname);
    QDomElement IAddNewDomainElement(QDomElement& IpNode,QString& Domain,QString& groupname);
    QDomElement IAddNewDomainElement(QDomElement& GroupNode,QString& Domain);

    bool ToHosts(QString fileName);
private:
    qint64 HostsWriteOneLine(QFile& file,const char* buf,qint64 len,bool iscomment=false);
    qint64 HostsWriteOneLine(QFile& file,const QString &str ,bool iscomment=false);
    //XML的DOC节点
    QDomDocument m_doc;
    //根节点
    QDomElement m_root;
    //用来保存或者打开xml的文件
    QString m_filename;
    //文档或者结构是否被修改过
    bool  m_modified;
    //当前是否处于打开状态
    bool  m_isopen;
    //是否是new的文件
    bool  m_isnewfile;

    //修复linux上 unable to resolve host xxxxxx,通过gethostname 127.0.0.1 增加进去
    void AddComputerHostName();


};

#endif // HOSTSXML

