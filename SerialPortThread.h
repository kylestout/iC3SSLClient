#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class SerialPortThread : public QThread
{
    Q_OBJECT

public:
    SerialPortThread(QObject *parent = 0);
    ~SerialPortThread();

    void transaction(const QString &portName, int waitTimeout, const QByteArray &request);
    void run();

signals:
    void response(const QByteArray &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    QString portName;
    QByteArray request;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
};

#endif // SERIALPORTTHREAD_H
