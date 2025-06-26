#include <QCoreApplication>
/*
QCoreApplication — класс в библиотеке Qt, который обеспечивает цикл обработки событий для консольных приложений
*/
#include "mytcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv); // инициализирует Qt-приложение (терминал)
    mytcpserver serv;
    return a.exec(); //запускает цикл обработки событий
}
