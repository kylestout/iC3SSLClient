#include <QApplication>
#include "client.h"
#include "ErrorLogFile.h"


void myMessageOutput(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    ErrorLogFile errorLog;
    QString sErrorMessage;
    QByteArray localMsg;

    switch (type)
    {
    case QtDebugMsg:
        sErrorMessage = QString("%1 Debug: %2\n")
                .arg(QDateTime::currentDateTime().toString())
                .arg(msg);
        // log to the error log file
        errorLog.logErrorMessage( sErrorMessage );
        break;

    case QtWarningMsg:
        sErrorMessage = QString("%1 Warning: %2 (%3, %4, %5)\n")
                .arg(QDateTime::currentDateTime().toString())
                .arg(msg)
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);

        // log to the error log file
        errorLog.logErrorMessage( sErrorMessage );
        break;

    case QtCriticalMsg:
        sErrorMessage = QString("%1 Critical Error: %2 (%3, %4, %5)\n")
                .arg(QDateTime::currentDateTime().toString())
                .arg(msg)
                .arg(context.file)
                .arg(context.line)
                .arg(context.function);

        // log to the error log file
        errorLog.logErrorMessage( sErrorMessage );

        // write to the console (Application Output)
        localMsg = sErrorMessage.toLocal8Bit();
        fprintf(stderr, "%s\r\n", localMsg.constData());
        fprintf(stderr, "Critical failure - Aborting\r\n" );
        fflush( stderr );

        // ABORT!
        abort();

        break;

    case QtFatalMsg:
        // qFatal() << "Error String"; - does not work so use qCritical() instead.
        break;
    }

    // write to the console (Application Output)
    localMsg = sErrorMessage.toLocal8Bit();
    fprintf(stderr, "%s", localMsg.constData());
    fflush( stderr );
}


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  qInstallMessageHandler(myMessageOutput);

  Client w;
  w.show();

  return a.exec();
}
