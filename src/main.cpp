//主函数入口
//

#include "MNIC.h"

#include <QApplication>

#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MNIC w;
    w.show();
    return a.exec();
}