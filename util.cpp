#include "util.h"
#include "time.h"


bool Util::IsValidDomainStr(const QString& DomainStr)
{
    bool bret=false;
    int len=DomainStr.length();
    do
    {
        if(len==0||len>MAX_DOMAIN_LEN)
        {
            break;
        }
        bret=true;
        for(int i=0;i<len;i++)
        {
            if(!IsValidDomainChar(DomainStr.at(i)))
            {
                bret=false;
                break;
            }
        }
    }while(false);

    return bret;
}

QString Util::timestr()
{
    time_t timet=time(NULL);
    QDateTime curtime=QDateTime::fromTime_t(timet);
    return curtime.toString("yyyy_MM_dd-HH_mm_ss");
}

bool Util::IsValidDomainChar(const QChar& ch)
{
    bool bret=true;
    do
    {
        if(Is0to9(ch))
        {
            break;
        }
        //
        if(IsLetter(ch))
        {
            break;
        }
        if(ch=='.'||ch=='-'||ch=='_')
        {
            break;
        }
        bret=false;
    }while(false);
    return bret;
}


//可以用正则来检测
bool Util::IsValidIpv4Str(const QString& ipStr)
{
    bool bret=false;
    do
    {
        int len=ipStr.length();
        if(len==0||len>15)
        {
            break;
        }
        QString part;
        int pcount=0;
        for(int i=0;i<len;i++)
        {
            QChar ch=ipStr.at(i);
            if(Util::Is0to9(ch))
            {
                part+=ch;
                if(part.length()>3)
                {
                    break;
                }
            }
            else if(ch=='.')
            {
                if(part.length()==0)
                {
                    break;
                }
                if(part.toInt()>255)
                {
                    break;
                }
                part="";
                pcount++;
            }
            else
            {

                break;
            }
        }
        if(pcount==3&&part.length())
        {
            bret=true;
        }
    }while(false);
    return bret;
}


bool Util::Is0to9(const QChar& ch)
{
    return ch>='0'&&ch<='9';
}
bool Util::IsLetter(const QChar& ch)
{
    return (ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z');
}


DomainValidator::DomainValidator(QObject * parent):QValidator(parent)
{

}

DomainValidator::~DomainValidator()
{

}

QValidator::State	DomainValidator::validate(QString & input, int & pos) const
{
    input=input.trimmed();
    QValidator::State state=QValidator::Acceptable;
    if(!Util::IsValidDomainStr(input))
    {
        pos=pos;
        state=QValidator::Invalid;
    }
    return state;
}

Ipv4Validator::Ipv4Validator(QObject * parent):QValidator(parent)
{

}

Ipv4Validator::~Ipv4Validator()
{

}

void Ipv4Validator::fixup(QString & input) const
{
    Q_UNUSED(input);
}

QValidator::State	Ipv4Validator::validate(QString & input, int & pos)const
{
    QValidator::State state=QValidator::Invalid;
    input=input.trimmed();
    int len=input.length();
    do
    {

        //ipv4最多255.255.255.255
        if(len>15)
        {
            break;
        }

        QString part;
        int i=0;
        for(;i<len;i++)
        {
            QChar ch=input.at(i);
            if(Util::Is0to9(ch))
            {
                part+=ch;
                //每个部分不能超过3 最多是255
                if(part.length()>3)
                {
                    break;
                }
                if(part.toInt()>255)
                {
                    pos=pos-3;
                    input.truncate(pos);
                    state=QValidator::Acceptable;
                    break;
                }
            }
            else if(ch=='.')
            {
                if(part.length()==0)
                {
                    break;
                }
                part="";
            }
            else
            {
                break;
            }
        }

        if(i==len)
        {
             state=QValidator::Acceptable;
        }



    }while(false);
    return state;
}

enDescValidator::enDescValidator(QObject * parent):QValidator(parent)
{

}

enDescValidator::~enDescValidator()
{

}

QValidator::State	enDescValidator::validate(QString & input, int & pos)const
{
    Q_UNUSED(pos);
    input=input.trimmed();
    int len=input.length();
    if(len>=MAX_DESC_LEN)
    {
        return QValidator::Invalid;
    }

    for(int i=0;i<len;i++)
    {
        QChar ch=input.at(i);
        if(!Util::IsLetter(ch)&&!Util::Is0to9(ch))
        {
            return QValidator::Invalid;
        }
    }
    return QValidator::Acceptable;
}

void    enDescValidator::fixup(QString & input) const
{
Q_UNUSED(input);
}
