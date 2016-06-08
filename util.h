#ifndef UTIL
#define UTIL
#include <QtCore/QString>
#include <QValidator>
#include <QDateTime>

//域名的最大长度
#ifndef MAX_DOMAIN_LEN
#define MAX_DOMAIN_LEN 64
#endif

#ifndef MAX_ATTRNAME_LEN
#define MAX_ATTRNAME_LEN 16
#define MAX_DESC_LEN     16
#endif

class Util
{
public:
    static bool IsValidDomainStr(const QString& DomainStr);
    static bool IsValidDomainChar(const QChar& ch);

    static bool IsValidIpv4Str(const QString& ipStr);

    static bool Is0to9(const QChar& ch);
    static bool IsLetter(const QChar& ch);
    static QString timestr();
};


class DomainValidator:public QValidator
{
public:
    DomainValidator(QObject * parent = 0);
    ~DomainValidator();
    virtual State	validate(QString & input, int & pos)const;
};

class Ipv4Validator:public QValidator
{
public:

    Ipv4Validator(QObject * parent = 0);
    ~Ipv4Validator();

    virtual State	validate(QString & input, int & pos)const;
    virtual void    fixup(QString & input) const;
};


class enDescValidator:public QValidator
{
public:

    enDescValidator(QObject * parent = 0);
    ~enDescValidator();

    virtual State	validate(QString & input, int & pos)const;
    virtual void    fixup(QString & input) const;
};


#endif // UTIL

