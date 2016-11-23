#ifndef DOORMANAGER_H
#define DOORMANAGER_H

#include <QObject>


#include "DataManager.h"
#include "SerialPortThread.h"

class DoorManager : public QObject
{
    Q_OBJECT

public:
    explicit DoorManager( QObject *parent = 0);

    void lockDoor(void);
    void unlockDoor(DataManager *pDataManager, QString sUserID );

    void turnLightOff( void );
    void turnLightOn( void );

    void setSerialPortName( QString sSerialPortName );
    void setWaitTimeoutMS( int iTimeoutMS );

    bool isSerialPortFound( void );

private slots:

    void handleResponse(QByteArray baResponse);
    void handleSerialPortError(QString sError);
    void handleTimeout(QString sMessage);

signals:

    void signalCommError( QString sErrorMessage );
    void signalDBError( QString sErrorMessage );

    void signalLightOn(void);
    void signalLightOff(void);
    void signalDoorLocked(void);
    void signalDoorUnlocked(void);

private:

    //------------------------------------------
    // Private Functions
    //------------------------------------------
    bool findSerialPort( void );
    void sendSerialRequest(QString sPortName, int iWaitTimeoutMS, QByteArray baRequest );
    QString byteArrayToHexString( QByteArray & buffer );

    //------------------------------------------
    // Private Data
    //------------------------------------------
    SerialPortThread m_SerialPort;

    QString m_sComPort;
    int     m_iWaitTimeoutMS;

    QByteArray m_baUnlockDoor;
    QByteArray m_baUnlockDoorAck;
    QByteArray m_baLockDoor;
    QByteArray m_baLockDoorAck;

    QByteArray m_baLightOn;
    QByteArray m_baLightOnAck;
    QByteArray m_baLightOff;
    QByteArray m_baLightOffAck;

    bool m_bWaitingForTimeout;
    bool m_bSerialPortFound;
};

#endif // DOORMANAGER_H
