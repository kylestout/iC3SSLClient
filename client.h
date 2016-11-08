#ifndef CLIENT_H
#define CLIENT_H

#include <QSslSocket>
#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include "QtJson.h"
#include "qcustomplot.h"
#include "iC3_Database.h"
#include "SerialPortThread.h"

using QtJson::JsonObject;
using QtJson::JsonArray;

static const QChar LEFT_CULY_BRACKET_CHARACTER( 0x7B );
static const QString REQUEST_STATUS_FILE ("./requests/status");
static const QString REQUEST_LOCK_FILE ("./requests/lock");
static const QString REQUEST_UNLOCK_FILE ("./requests/unlock");
static const QString REQUEST_LIGHT_ON_FILE ("./requests/lighton");
static const QString REQUEST_LIGHT_OFF_FILE ("./requests/lightoff");
static const QString REQUEST_DUTY_CYCLE_FILE ("./requests/dutycycle");
static const QString REQUEST_PELTIER_HIGH_FILE ("./requests/peltierhigh");
static const QString REQUEST_PELTIER_LOW_FILE ("./requests/peltierlow");
static const QString REQUEST_PELTIER_OFF_FILE ("./requests/peltieroff");
static const QString IMAGE_LED_OFF ("./images/led-off.png");
static const QString IMAGE_LED_ON ("./images/led-on.png");


static const double UNUSED_PROBE_VALUE       = 99.9;
static const int    TIMEOUT_GRAPH_UPDATE_SEC = 10;
static const int    GRAPH_X_AXIS_MINUTES     = 60;
static const int    TIMEOUT_FLUKE_TEMP_UPDATE_SEC = 3;


enum eFlukeTcCommands
{
    eFLUKE_TC_UNKNOWN      =-1,
    eFLUKE_TC_1            = 0,
    eFLUKE_TC_2            = 1
};

enum eRTDNumber
{
    eRTD_UNKNOWN =-1,
    eRTD1        = 0,
    eRTD2        = 1,
    eRTD3        = 2,
    eRTD4        = 3,
    eRTD5        = 4,
};


namespace Ui {
  class Client;
}

class Client : public QMainWindow
{
  Q_OBJECT

public:
  explicit Client(QWidget *parent = 0);
  ~Client();


    void turnLightOff( void );
    void turnLightOn( void );

    void setSerialPortName( QString sSerialPortName );
    void setWaitTimeoutMS( int iTimeoutMS );

    bool isSerialPortFound( void );

    void sendCalibrationRequest(eRTDNumber eRTDNum , double dRTDOffsetVal);

protected slots:
  // Slots to receive signals from UI
  void connectDisconnectButtonPressed();
  void sendButtonPressed();

  // Slots to receive signals from QSslSocket
  void connectedToServer();
  void sslErrors(const QList<QSslError> &errors);
  void receiveMessage();
  void connectionClosed();
  void socketError();
  void sendMessageTimeout();
  QString getPayload( QString sData, QChar cStartOfPayload );
  void readRequestFile( QString sFileName, QString & sMemVar );
  void realtimeDataSlot();
  void flukeTempTimeout();
  void getFlukeTemp1();
  void getFlukeTemp2();

private slots:
  void on_sendContButton_clicked();
  void on_dial_valueChanged(int value);
  void on_stopButton_clicked();

  void on_button_lock_clicked();

  void on_button_unlock_clicked();

  void on_button_light_on_clicked();

  void on_button_light_off_clicked();

  void on_button_peltier_high_clicked();

  void on_button_peltier_low_clicked();

  void on_button_peltier_stop_clicked();


  void handleResponse(QByteArray baResponse);
  void handleSerialPortError(QString sError);
  void handleTimeout(QString sMessage);

  void on_button_match_primary_clicked();

  void on_button_primary_up_clicked();

  void on_button_primary_down_clicked();

  void on_button_control_up_clicked();

  void on_button_control_down_clicked();

private:
  QSslSocket socket;
  Ui::Client *ui;
  int m_iCurrentDialValue;
  QTimer * sendMessageTimer;
  int msgsSentCount;
  int msgsRecCount;
  int contConnects;
  int contDisconnects;
  bool conButtonClicked;
  QList<QString> messages;
  int bufferMsgCount;
  QString m_sRequest_Status;
  QString m_sRequest_Lock;
  QString m_sRequest_Unlock;
  QString m_sRequest_LightOn;
  QString m_sRequest_LightOff;
  QString m_sRequest_DutyCycle;
  QString m_sRequest_PeltierHigh;
  QString m_sRequest_PeltierLow;
  QString m_sRequest_PeltierOff;

  QPixmap m_ledON;
  QPixmap m_ledOFF;

  QTimer dataTimer;

  QTimer flukeTimer;
  double m_dPrimary;
  double m_dSecondary;
  double m_dControl;
  double m_dCompressor;
  double m_dFlukeChannel1;
  double m_dFlukeChannel2;
  iC3_Database db;

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


  eFlukeTcCommands m_eLastFlukeMsgSent;

  double m_dRTD4_OffsetValue;
  double m_dRTD5_OffsetValue;


};

#endif // CLIENT_H
