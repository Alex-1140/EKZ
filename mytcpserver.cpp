#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include<QString>
#include <QList>

mytcpserver::~mytcpserver(){
    mTcpServer->close();
}

mytcpserver::mytcpserver(QObject *parent): QObject (parent)
{
    mTcpServer = new QTcpServer(this);
    /*Передача this в конструктор — это указатель на текущий объект MyTcpServer.

Зачем передавать this?
Это родительский объект (parent) для механизма Qt-иерархии.

Когда родитель (MyTcpServer) удаляется, Qt автоматически удалит и дочерний объект (mTcpServer).

Без this пришлось бы вручную вызывать delete mTcpServer в деструкторе.*/

    connect(mTcpServer, &QTcpServer::newConnection, this, &mytcpserver::slotNewConnection);

/*
connect - Это статический метод класса QObject, который связывает Сигнал и Слот


Метод принимает 4 основных параметра:

Отправитель сигнала - Объект, который будет "генерировать" сигнал (в данном случае — сервер)
Сигнал - Указатель на сигнал "новое подключение" класса QTcpServer
Получатель	-	Объект, чей слот будет обрабатывать сигнал (текущий объект MyTcpServer)
Слот - Указатель на метод-обработчик в текущем классе

Как работает сигнал newConnection?
    срабатывает: При каждом новом входящем TCP-подключении к серверу.
    Что делает: "Кричит" всем подключенным слотам: "Эй, у меня новый клиент!".
    Где объявлен: В классе QTcpServer:


Когда у mTcpServer появится новое подключение, вызови метод slotNewConnection текущего объекта (this)
*/

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
}



void mytcpserver::slotNewConnection(){

    if (mTcpSocket.size() >= 3)
    {
        QTcpSocket *news = mTcpServer->nextPendingConnection();
        news->write("Server is busy.\r\n");
        news->disconnectFromHost();
        return;
    }
    QTcpSocket *clien = mTcpServer->nextPendingConnection();
    mTcpSocket.append(clien);
    scores[clien] = 0;

    clien->write("Hello client.\r\n");


    QString message = QString("Количество пользователей: %1\r\n").arg(mTcpSocket.size());
    for (QTcpSocket *socket : mTcpSocket) {
        if (socket != clien) { // Не отправляем уведомление самому новому клиенту
            socket->write(message.toUtf8());
        }
    }
    qDebug()<< "Количество пользователей"<<mTcpSocket.size();

    connect(clien, &QTcpSocket::readyRead,this,&mytcpserver::slotServerRead);
    connect(clien,&QTcpSocket::disconnected,this,&mytcpserver::slotClientDisconnected);


    if (mTcpSocket.size() == 2) {
        gameActive = true;
        currentQuestion = 0;
        sendQuestion();
    }


}


void mytcpserver::slotServerRead()
{
    if (!gameActive) {
        // Игнорируем сообщения, если игра не активна
        QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
        if (client) {
            client->readAll(); // Очищаем буфер
            client->write("Игра еще не началась. Подождите игроков.\r\n");
        }
        return;
    }

    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QString answer = QString(client->readAll()).trimmed();

    if (!answeredClients[client]) {
        checkAnswer(answer, client);
    }

}



void mytcpserver::slotClientDisconnected(){
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        mTcpSocket.removeOne(client);
        client->deleteLater();
    }
}


void mytcpserver::checkAnswer(const QString &answer, QTcpSocket *client)
{
    if (currentQuestion < answers.size()) {
        QString correctAnswer = answers[currentQuestion];
        if (answer==correctAnswer) {
            scores[client]++;
            client->write("Правильно!\r\n");
        } else {
            client->write(QString("Неверно! Правильный ответ: %1\r\n").arg(correctAnswer).toUtf8());
        }
    }

    answeredClients[client] = true;
    checkAllAnswered();
}

void mytcpserver::checkAllAnswered()
{
    bool allAnswered = true;
    for (QTcpSocket *socket : mTcpSocket) {
        if (!answeredClients.value(socket, false)) {
            allAnswered = false;
            break;
        }
    }

    if (allAnswered) {
        answeredClients.clear();
        currentQuestion++;
        sendQuestion();
    }
}

void mytcpserver::sendQuestion()
{
    if (currentQuestion < questions.size()) {
        QString question = QString("Вопрос: %1\r\n")
            .arg(questions[currentQuestion]);
        qDebug()<<question;
        qDebug()<<"Ответ: "<<answers;
        for (QTcpSocket *socket : mTcpSocket) {
            socket->write(question.toUtf8());
        }
    } else {
        endGame();
    }
}

void mytcpserver::endGame()
{
    gameActive = false;

    QTcpSocket *winner = nullptr;
    int maxScore = 0;
    bool draw = false;

    for (QTcpSocket *socket : mTcpSocket) {
        if (scores[socket] > maxScore) {
            maxScore = scores[socket];
            winner = socket;
            draw = false;
        } else if (scores[socket] == maxScore && maxScore > 0) {
            draw = true;
        }
    }

    for (QTcpSocket *socket : mTcpSocket) {
        if (draw) {
            socket->write("win!\r\n");
        } else if (socket == winner) {
            socket->write("try again!\r\n");
        } else {
            socket->write("win!\r\n");
        }


        socket->disconnectFromHost();
    }
    qDebug()<<"Game over";
    // Clean up
    mTcpSocket.clear();
    scores.clear();
    answeredClients.clear();
}
