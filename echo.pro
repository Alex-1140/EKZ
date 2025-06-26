QT -= gui #отключаем gui, т.е. консольное приложение
QT += core
QT += network #Для работы с сетью, подключаем драйвера


CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS # Если есть ошибки, то проект все равно запустится

SOURCES += \
        main.cpp \
        mytcpserver.cpp

HEADERS +=\
        mytcpserver.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
