#include "DoorManager.h"
#include <QDebug>
#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
DoorManager::DoorManager(QObject *parent) :
    QObject(parent),
    m_sComPort("COM5"),
    m_iWaitTimeoutMS(500),
    m_bWaitingForTimeout(false)
{
    m_bSerialPortFound = findSerialPort();

    // ---------------------------------------------------------
    // set up the unlock door serial port command
    //----------------------------------------------------------
    const char UNLOCK_DOOR_COMMAND[] = {(char)0x02,
                                        (char)0x08,
                                        (char)0xA0,
                                        (char)0x01,
                                        (char)0x00,
                                        (char)0xFF,
                                        (char)0xFF,
                                        (char)0x58,
                                        (char)0xFD,
                                        (char)0x00,
                                        (char)0x03};
    m_baUnlockDoor.clear();
    m_baUnlockDoor.append( UNLOCK_DOOR_COMMAND, 11 );

    const char UNLOCK_DOOR_ACK[] = {(char)0x02,
                                    (char)0x08,
                                    (char)0xA1,
                                    (char)0x01,
                                    (char)0x00,
                                    (char)0xFF,
                                    (char)0xFF,
                                    (char)0x57,
                                    (char)0xFD,
                                    (char)0x00,
                                    (char)0x03};
    m_baUnlockDoorAck.clear();
    m_baUnlockDoorAck.append( UNLOCK_DOOR_ACK, 11 );

    // ---------------------------------------------------------
    // set up the lock door serial port command
    //----------------------------------------------------------
    const char LOCK_DOOR_COMMAND[]   = {(char)0x02,
                                        (char)0x08, \
                                        (char)0xA0,
                                        (char)0x01,
                                        (char)0x00,
                                        (char)0xFE,
                                        (char)0xFF,
                                        (char)0x59,
                                        (char)0xFD,
                                        (char)0x01,
                                        (char)0x03};
    m_baLockDoor.clear();
    m_baLockDoor.append( LOCK_DOOR_COMMAND, 11 );

    const char LOCK_DOOR_ACK[]   = {(char)0x02,
                                    (char)0x08, \
                                    (char)0xA1,
                                    (char)0x01,
                                    (char)0x00,
                                    (char)0xFE,
                                    (char)0xFF,
                                    (char)0x58,
                                    (char)0xFD,
                                    (char)0x01,
                                    (char)0x03};
    m_baLockDoorAck.clear();
    m_baLockDoorAck.append( LOCK_DOOR_ACK, 11 );


    // ---------------------------------------------------------
    // set up the light On serial port command
    //----------------------------------------------------------
    const char LIGHT_ON_COMMAND[]   = {(char)0x02,
                                       (char)0x0A, \
                                       (char)0xA0,
                                       (char)0x01,
                                       (char)0x00,
                                       (char)0xFE,
                                       (char)0xFF,
                                       (char)0x57,
                                       (char)0xFD,
                                       (char)0x01,
                                       (char)0x03};
    m_baLightOn.clear();
    m_baLightOn.append( LIGHT_ON_COMMAND, 11 );

    const char LIGHT_ON_ACK[]       = {(char)0x02,
                                       (char)0x0A, \
                                       (char)0xA1,
                                       (char)0x01,
                                       (char)0x00,
                                       (char)0xFE,
                                       (char)0xFF,
                                       (char)0x56,
                                       (char)0xFD,
                                       (char)0x01,
                                       (char)0x03};
    m_baLightOnAck.clear();
    m_baLightOnAck.append( LIGHT_ON_ACK, 11 );

    // ---------------------------------------------------------
    // set up the light Off serial port command
    //----------------------------------------------------------
    const char LIGHT_OFF_COMMAND[]   = {(char)0x02,
                                        (char)0x0A, \
                                        (char)0xA0,
                                        (char)0x01,
                                        (char)0x00,
                                        (char)0xFF,
                                        (char)0xFF,
                                        (char)0x56,
                                        (char)0xFD,
                                        (char)0x00,
                                        (char)0x03};
    m_baLightOff.clear();
    m_baLightOff.append( LIGHT_OFF_COMMAND, 11 );

    const char LIGHT_OFF_ACK[]      = {(char)0x02,
                                       (char)0x0A, \
                                       (char)0xA1,
                                       (char)0x01,
                                       (char)0x00,
                                       (char)0xFF,
                                       (char)0xFF,
                                       (char)0x55,
                                       (char)0xFD,
                                       (char)0x00,
                                       (char)0x03};
    m_baLightOffAck.clear();
    m_baLightOffAck.append( LIGHT_OFF_ACK, 11 );

    // ---------------------------------------------------------
    connect(&m_SerialPort, SIGNAL(response(QByteArray)), this, SLOT(handleResponse(QByteArray)));
    connect(&m_SerialPort, SIGNAL(error(QString)), this, SLOT(handleSerialPortError(QString)));
    connect(&m_SerialPort, SIGNAL(timeout(QString)), this, SLOT(handleTimeout(QString)));
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::sendSerialRequest( QString sPortName, int iWaitTimeoutMS, QByteArray baRequest )
{
    m_SerialPort.transaction( sPortName, iWaitTimeoutMS, baRequest );
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::handleResponse( QByteArray baResponse )
{
    m_bWaitingForTimeout = false;

    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    qDebug() << "   Serial Port Response";
    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    qDebug() << byteArrayToHexString( baResponse );
    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

    if( baResponse == m_baLightOffAck )
    {
        qDebug() << "++ received Light Off Ack";
        emit signalLightOff(); 
    }
    else if ( baResponse == m_baLightOnAck )
    {
        qDebug() << "++ received Light On Ack";
        emit signalLightOn();
    }
    else if ( baResponse == m_baUnlockDoorAck )
    {
        qDebug() << "++ received Door Unlocked Ack";
        emit signalDoorUnlocked(); 
    }
    else if ( baResponse == m_baLockDoorAck )
    {
        qDebug() << "++received Door Locked Ack";
        emit signalDoorLocked();
    }
    else
    {
        qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
        qDebug() << "   Serial Port Response";
        qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
        qDebug() << byteArrayToHexString( baResponse );
        qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    }
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::handleSerialPortError( QString sError )
{
    qWarning() << sError;
    qDebug() << "Serial Port Error: " << sError;
    m_bWaitingForTimeout = false;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::handleTimeout( QString sMessage )
{
    qDebug() << "Serial Port Timeout: " << sMessage;
    m_bWaitingForTimeout = false;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::lockDoor( void )
{
    if ( m_bWaitingForTimeout == false )
    {
        qDebug() << "===========================================================";
        qDebug() << "   Locking Door: Serial Port Send";
        qDebug() << "===========================================================";
        qDebug() << byteArrayToHexString( m_baLockDoor );
        qDebug() << "===========================================================";

        sendSerialRequest(m_sComPort, m_iWaitTimeoutMS, m_baLockDoor );
        m_bWaitingForTimeout = true;
    }
    else
    {
        qWarning() << "Can not send the unlockDoor command - waiting for previous serial port command response";
    }
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::unlockDoor(DataManager * pDataManager, QString sUserID)
{
    if ( m_bWaitingForTimeout == false )
    {
        qDebug() << "===========================================================";
        qDebug() << "   Unlocking Door: Serial Port Send";
        qDebug() << "===========================================================";
        qDebug() << byteArrayToHexString( m_baUnlockDoor );
        qDebug() << "===========================================================";

        pDataManager->recordDoorOpening( sUserID );
        sendSerialRequest(m_sComPort, m_iWaitTimeoutMS, m_baUnlockDoor );
        m_bWaitingForTimeout = true;
    }
    else
    {
        qWarning() << "Can not send the unlockDoor command - waiting for previous serial port command response";
    }
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::turnLightOn(void)
{
    if ( m_bWaitingForTimeout == false )
    {
        qDebug() << "===========================================================";
        qDebug() << "  Sending Light On";
        qDebug() << "===========================================================";
        qDebug() << byteArrayToHexString( m_baLightOn );
        qDebug() << "===========================================================";

        sendSerialRequest(m_sComPort, m_iWaitTimeoutMS, m_baLightOn );
        m_bWaitingForTimeout = true;
    }
    else
    {
        qWarning() << "Can not send the light on command - waiting for previous serial port command response";
    }
}


//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::turnLightOff( void )
{
    if ( m_bWaitingForTimeout == false )
    {
        qDebug() << "===========================================================";
        qDebug() << "  Sending Light Off";
        qDebug() << "===========================================================";
        qDebug() << byteArrayToHexString( m_baLightOff );
        qDebug() << "===========================================================";

        sendSerialRequest(m_sComPort, m_iWaitTimeoutMS, m_baLightOff );
        m_bWaitingForTimeout = true;
    }
    else
    {
        qWarning() << "Can not send the light off command - waiting for previous serial port command response";
    }
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::setSerialPortName( QString sSerialPortName )
{
    m_sComPort = sSerialPortName;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void DoorManager::setWaitTimeoutMS( int iTimeoutMS )
{
    m_iWaitTimeoutMS = iTimeoutMS;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
bool DoorManager::findSerialPort( void )
{
    bool bRC = false;
    QSerialPort serial;
    QString sPortName;

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        sPortName = info.portName();
        qDebug() << "Trying Serial Port: " << sPortName;
        serial.close();
        serial.setPortName(sPortName);

        bRC = serial.open(QIODevice::ReadWrite);

        if ( bRC == true )
        {
            setSerialPortName( sPortName );
            qDebug() << "Using Serial Port: " << sPortName;
            serial.close();
            break;
        }
        else
        {
            qDebug() << "Serial Port: " << sPortName << " not connected.";
        }
    }

    serial.close();

    return bRC;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
bool DoorManager::isSerialPortFound( void )
{
    return m_bSerialPortFound;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
QString DoorManager::byteArrayToHexString( QByteArray & buffer )
{
    int iIndex;
    QString sReturnString;
    QString sTempString = buffer.toHex().toUpper();
    int iBuffSize = sTempString.size();

    for ( iIndex = 0; iIndex < iBuffSize; iIndex+=2 )
    {
        //sReturnString.append("0x");
        sReturnString.append( sTempString.mid(iIndex, 2) );
        sReturnString.append(" ");
    }

    return sReturnString;
}
