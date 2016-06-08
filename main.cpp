#include "mainui.h"
#include <QApplication>
#include "stdio.h"
#include <QTextCodec>
#ifdef _WIN32
#include "Windows.h"
#else
#endif

int main(int argc, char *argv[])
{
    LOGINIT(LOGTOFILE,LOGL_DEBUG,"log.log");
#ifdef _WIN32
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$","w",stdout);
    printf("Debug Mode\n");
#endif
#endif
    QApplication a(argc, argv);
    MainUi w;
    w.show();

    return a.exec();
}
