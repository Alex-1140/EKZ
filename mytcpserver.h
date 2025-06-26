#ifndef MYTCPSERVER_H // если не определна константа MYTCPSERVER_H, то определить ее и выполнить прочитать код до #endif
//используется для защиты от повторного включения . Это предотвращает ошибки компиляции из-за множественного определения классов/функций.


#define MYTCPSERVER_H
#include <QObject> /* базовый класс для всех объектов Qt: Обеспечивает систему сигналов и слотов */
#include <QTcpServer> /*серверная часть TCP соединения. Позволяет принимать входящие соединения*/
#include <QTcpSocket> /*клиентская часть TCP соединения. Предоставляет интерфейс для работы с TCP-сокетом*/

#include <QtNetwork> //главный сетевой модуль Qt. Включает все основные сетевые классы
#include <QByteArray> //класс для работы с бинарными данными
#include <QDebug> //модуль для отладочного вывода
#include <QList>
#include <QMap>

class mytcpserver: public QObject
{
    Q_OBJECT
public:
    explicit mytcpserver(QObject *parent = nullptr);
    /* Ключевое слово explicit в C++ запрещает компилятору делать неявные (автоматические) преобразования типов. */
    ~mytcpserver();
    QString Cezar(QString res);
public slots:
/*
Это механизм в Qt для связи между объектами, когда один объект может "кричать" (сигнал), а другой — "реагировать" (слот).
Чтобы объекты могли общаться без жёсткой зависимости друг от друга (как в соцсетях: подписался на событие — получил уведомление).Отправитель сигнала (плита) не содержит информации о получателе (чайнике).
Может быть множество получателей одного сигнала.

Сигнал — это сообщение, которое передается, чтобы сообщить об изменении состояния объекта. Сигнал может нести информацию о произошедшем изменении.

Слот — это специальная функция, вызываемая в ответ на определенный сигнал. Поскольку слоты — это функции, они содержат логику для выполнения определенного действия.



*/

    void slotNewConnection();
    void slotClientDisconnected();
    void slotServerRead();

private:
    QTcpServer *mTcpServer;
    QList<QTcpSocket*> mTcpSocket;
    QMap<QTcpSocket*, int> scores;
    int currentQuestion = 0;
    bool gameActive = false;
    QMap<QTcpSocket*, bool> answeredClients;

    void sendQuestion();
    void endGame();
    void checkAnswer(const QString &answer, QTcpSocket *client);
    void checkAllAnswered();

    const QList<QString> questions = {
        "Столица Франции?",
        "2*2=?",
        "Кто написал Евгения Онегина?"
    };

    const QList<QString> answers = {
        "Париж",
        "4",
        "Пушкин"
    };


};

#endif // MYTCPSERVER_H
