#include <QDateTime>
#include <QMessageBox>
#include "client.h"
#include "ui_client.h"
#include <QList>
#include "qcustomplot.h"

#include <QtSerialPort>
#include <QtSerialPort/QSerialPortInfo>


Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client),
    m_iCurrentDialValue(1000),
    msgsSentCount(0),
    msgsRecCount(0),
    contConnects(0),
    contDisconnects(0),
    conButtonClicked(false),
    m_sRequest_Status(""),
    m_sRequest_Lock(""),
    m_sRequest_Unlock(""),
    m_sRequest_LightOn(""),
    m_sRequest_LightOff(""),
    m_sRequest_DutyCycle(""),
    m_sRequest_PeltierHigh(""),
    m_sRequest_PeltierLow(""),
    m_sRequest_PeltierOff(""),
    m_dPrimary(0.0),
    m_dSecondary(0.0),
    m_dControl(0.0),
    m_dCompressor(0.0),
    m_dFlukeChannel1(0.0),
    m_dFlukeChannel2(0.0),
    m_sComPort("COM5"),
    m_iWaitTimeoutMS(3000),
    m_bWaitingForTimeout(false),
    m_eLastFlukeMsgSent(eFLUKE_TC_UNKNOWN),
    m_dRTD4_OffsetValue(0.0),
    m_dRTD5_OffsetValue(0.0),
    m_bCompressorState(false),
    m_sDeviceType(""),
    m_pCalibrationManager(NULL)
{
  ui->setupUi(this);

  qDebug() << "\n\n\n\n";
  qDebug() << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
  qDebug() << "SYSTEM START";
  qDebug() << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
  qDebug() << "\n\n\n\n";

  // Check for SSL support.  If SSL support is not available, show a
  // message to the user describing what to do to enable SSL support.
  if (QSslSocket::supportsSsl())
  {
    ui->connectDisconnectButton->setEnabled(true);
  }
  else
  {
    QString noSslMsg = QString("%1\n%2")
        .arg("*** Your version of Qt does support SSL ***")
        .arg("You must obtain a version of Qt that has SSL"
             "support enabled.  If you believe that your "
             "version of Qt has SSL support enabeld, you may "
             "need to install the OpenSSL run-time libraries.");

    ui->chatDisplayTextEdit->setText(noSslMsg);
  }

  // QSslSocket emits the encrypted() signal after the encrypted connection is established
  connect(&socket, SIGNAL(encrypted()), this, SLOT(connectedToServer()));

  // Report any SSL errors that occur
  connect(&socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErrors(const QList<QSslError> &)));

  connect(&socket, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
  connect(&socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
  connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
  // start oneMinTimer
  sendMessageTimer = new QTimer(this);
  connect(sendMessageTimer, SIGNAL(timeout()), this, SLOT(sendMessageTimeout()));

  ui->dialLabel->setText(QString::number(m_iCurrentDialValue) + " ms");

  readRequestFile(REQUEST_STATUS_FILE, m_sRequest_Status);
  readRequestFile(REQUEST_LOCK_FILE, m_sRequest_Lock);
  readRequestFile(REQUEST_UNLOCK_FILE, m_sRequest_Unlock);
  readRequestFile(REQUEST_LIGHT_ON_FILE, m_sRequest_LightOn);
  readRequestFile(REQUEST_LIGHT_OFF_FILE, m_sRequest_LightOff);
  readRequestFile(REQUEST_DUTY_CYCLE_FILE, m_sRequest_DutyCycle);
  readRequestFile(REQUEST_PELTIER_HIGH_FILE, m_sRequest_PeltierHigh);
  readRequestFile(REQUEST_PELTIER_LOW_FILE, m_sRequest_PeltierLow);
  readRequestFile(REQUEST_PELTIER_OFF_FILE, m_sRequest_PeltierOff);

  QSize mySize = ui->led_door_alarm->size();

  QImage ledOFF = QImage( IMAGE_LED_OFF );
  QImage ledON = QImage( IMAGE_LED_ON );
  m_ledON = (QPixmap::fromImage(ledON)).scaled(mySize);
  m_ledOFF = (QPixmap::fromImage(ledOFF)).scaled(mySize);

  ui->button_control_down->setEnabled(false);
  ui->button_control_up->setEnabled(false);
  ui->button_light_off->setEnabled(false);
  ui->button_light_on->setEnabled(false);
  ui->button_lock->setEnabled(false);
  ui->button_unlock->setEnabled(false);
  ui->button_match_primary->setEnabled(false);
  ui->button_peltier_low->setEnabled(false);
  ui->button_peltier_high->setEnabled(false);
  ui->button_peltier_stop->setEnabled(false);
  ui->button_primary_down->setEnabled(false);
  ui->button_primary_up->setEnabled(false);
  ui->button_auto_cal->setEnabled(false);

  ui->led_power_state->setPixmap(m_ledOFF);
  ui->led_battery_state->setPixmap(m_ledOFF);
  ui->led_door_state->setPixmap(m_ledOFF);
  ui->led_peltier_active->setPixmap(m_ledOFF);
  ui->led_door_alarm->setPixmap(m_ledOFF);
  ui->led_primary_probe->setPixmap(m_ledOFF);
  ui->led_secondary_probe->setPixmap(m_ledOFF);
  ui->led_control_probe->setPixmap(m_ledOFF);
  ui->led_compressor_probe->setPixmap(m_ledOFF);
  ui->led_lock_state->setPixmap(m_ledOFF);
  ui->led_defrost_active->setPixmap(m_ledOFF);
  ui->led_calibrated->setPixmap(m_ledOFF);

  ui->lcd_primary->display("---");
  ui->lcd_secondary->display("---");
  ui->lcd_control->display("---");
  ui->lcd_compressor->display("---");
  ui->lcd_ac->display("---");
  ui->lcd_battery->display("---");
  ui->lcd_max->display("---");
  ui->lcd_min->display("---");
  ui->lcd_primary_offset->display("---");
  ui->lcd_control_offset->display("---");
  ui->lcd_fluke_1->display("---");
  ui->lcd_fluke_2->display("---");

//////////////////////////


  ui->graph->addGraph()->setName("Primary");
  ui->graph->graph(0)->setPen(QPen(QColor(40, 110, 255))); // blue line

  ui->graph->addGraph()->setName("Secondary");
  ui->graph->graph(1)->setPen(QPen(QColor(255, 110, 40))); // red line

  ui->graph->addGraph()->setName("Control");
  ui->graph->graph(2)->setPen(QPen(QColor(147, 214, 85))); // green line

  ui->graph->addGraph()->setName("Compressor");
  ui->graph->graph(3)->setPen(QPen(QColor(255, 251, 71))); // yellow line

  ui->graph->legend->setVisible(true);

  ui->graph->setBackground(Qt::gray);

  ui->graph_fluke->addGraph()->setName("Channel 1");
  ui->graph_fluke->graph(0)->setPen(QPen(QColor(40, 110, 255))); // blue line

  ui->graph_fluke->addGraph()->setName("Channel 2");
  ui->graph_fluke->graph(1)->setPen(QPen(QColor(147, 214, 85))); // green line

  ui->graph_fluke->legend->setVisible(true);

  ui->graph_fluke->setBackground(Qt::gray);


  // create and prepare a text layout element:
  QCPTextElement *legendTitle = new QCPTextElement(ui->graph);
  legendTitle->setLayer(ui->graph->legend->layer()); // place text element on same layer as legend, or it ends up below legend
  legendTitle->setFont(QFont("sans", 9, QFont::Bold));

  // create and prepare a text layout element:
  QCPTextElement *legendTitle2 = new QCPTextElement(ui->graph_fluke);
  legendTitle2->setLayer(ui->graph_fluke->legend->layer()); // place text element on same layer as legend, or it ends up below legend
  legendTitle2->setFont(QFont("sans", 9, QFont::Bold));


  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker->setTimeFormat("%h:%m:%s");
  ui->graph->xAxis->setTicker(timeTicker);
  ui->graph->axisRect()->setupFullAxesBox();
  ui->graph->yAxis->setRange(0.0, 10.0);

  QSharedPointer<QCPAxisTickerTime> timeTicker2(new QCPAxisTickerTime);
  timeTicker2->setTimeFormat("%h:%m:%s");
  ui->graph_fluke->xAxis->setTicker(timeTicker2);
  ui->graph_fluke->axisRect()->setupFullAxesBox();
  ui->graph_fluke->yAxis->setRange(0.0, 10.0);


  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->graph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->graph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph->yAxis2, SLOT(setRange(QCPRange)));

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->graph_fluke->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph_fluke->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->graph_fluke->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph_fluke->yAxis2, SLOT(setRange(QCPRange)));

  db.openDatabase();

  connect(&flukeTimer, SIGNAL(timeout()), this, SLOT(flukeTempTimeout()));

  m_bSerialPortFound = findSerialPort();

  // ---------------------------------------------------------
  connect(&m_SerialPort, SIGNAL(response(QByteArray)), this, SLOT(handleResponse(QByteArray)));
  connect(&m_SerialPort, SIGNAL(error(QString)), this, SLOT(handleSerialPortError(QString)));
  connect(&m_SerialPort, SIGNAL(timeout(QString)), this, SLOT(handleTimeout(QString)));

}

Client::~Client()
{
  if (socket.isOpen())
  {
    socket.close();
  }
  delete ui;
}

void Client::readRequestFile( QString sFileName, QString & sMemVar )
{
    QFile file;
    if(file.isOpen())
    {
        file.close();
    }

    file.setFileName(sFileName);

    if((file.exists()))
    {
        if((file.open(QIODevice::ReadOnly)))
        {
            QTextStream readData(&file);
            while (!readData.atEnd())
            {
                sMemVar += readData.readLine();
                sMemVar += "\n";
            }
        }
        else
        {
            qDebug() << "Unable to open file: %d" << file.error();
        }
    }
    else
    {
      qDebug() << sFileName << " - File does not exists";
    }

    file.close();
}

void Client::connectDisconnectButtonPressed()
{
  ui->connectDisconnectButton->setEnabled(false);

  if (socket.state() == QAbstractSocket::UnconnectedState)
  {
    // Initiate an SSL connection to the chat server.
    socket.connectToHostEncrypted(ui->hostnameLineEdit->text(), ui->portSpinBox->value());
  }
  else
  {
    socket.close();
  }
}

void Client::sendButtonPressed()
{
  QString message = ui->inputLineEdit->toPlainText();
  if (!message.isEmpty())
  {
    message += '\n';
    socket.write(message.toLocal8Bit().constData());
    msgsSentCount++;
    ui->msgsSentLabel->setText(QString::number(msgsSentCount));
  }

  ui->chatDisplayTextEdit->clear();
}
void Client::connectedToServer()
{
  ui->connectDisconnectButton->setText("Disconnect");
  ui->connectDisconnectButton->setEnabled(true);
  ui->inputLineEdit->setEnabled(true);
  ui->sendButton->setEnabled(true);
  ui->sendContButton->setEnabled(true);
  ui->stopButton->setEnabled(true);


  ui->button_control_down->setEnabled(true);
  ui->button_control_up->setEnabled(true);
  ui->button_light_off->setEnabled(true);
  ui->button_light_on->setEnabled(true);
  ui->button_lock->setEnabled(true);
  ui->button_unlock->setEnabled(true);
  ui->button_match_primary->setEnabled(true);
  ui->button_peltier_low->setEnabled(true);
  ui->button_peltier_high->setEnabled(true);
  ui->button_peltier_stop->setEnabled(true);
  ui->button_primary_down->setEnabled(true);
  ui->button_primary_up->setEnabled(true);
  ui->button_auto_cal->setEnabled(true);



  ui->chatDisplayTextEdit->clear();

  if (conButtonClicked)
  {
      contConnects++;
//      ui->numConLabel->setText(QString::number(contConnects));
  }

}

// Process SSL errors
void Client::sslErrors(const QList<QSslError> &errors)
{
    if (conButtonClicked)
    {
        socket.ignoreSslErrors();
        return;
    }

  QString errorStrings;
  foreach (QSslError error, errors)
  {
    errorStrings += error.errorString(); if (socket.state() == QAbstractSocket::UnconnectedState)
    {
      // Initiate an SSL connection to the chat server.
      socket.connectToHostEncrypted(ui->hostnameLineEdit->text(), ui->portSpinBox->value());
    }
    else
    {
      socket.close();
    }
    if (error != errors.last())
    {
      errorStrings += '\n';
    }
  }

  // Display error details to user and ask for permission to proceed anyway
  QMessageBox::StandardButton result = QMessageBox::question(this, "SSL Errors",
    QString("The following errors were encountered while negotiating the SSL connection:\n\n%1\n\nProceed anyway?").arg(errorStrings),
    QMessageBox::Yes|QMessageBox::No);
  if (result == QMessageBox::Yes)
  {
    socket.ignoreSslErrors();
  }
}

void Client::receiveMessage()
{
  //if (socket.canReadLine())
  //{

    QString myString = QString::fromUtf8(socket.readAll().constData());
    //qDebug() << "myString: " << myString;

    ui->chatDisplayTextEdit->append(/*QString("[%1] %2")
                                    .arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz ap"))
                                    .arg(*/myString/*)*/);

    if( myString.contains("200", Qt::CaseInsensitive) )
    {
        msgsRecCount++;
        ui->msgsRecLabel->setText(QString::number(msgsRecCount));
    }

    if(myString.contains(LEFT_CULY_BRACKET_CHARACTER))
    {
        bool bRC = false;
        QString sPayload = getPayload(myString, LEFT_CULY_BRACKET_CHARACTER);
        JsonObject result = QtJson::parse(sPayload, bRC).toMap();

        QString s;
        s = result["primaryProbeTemp"].toString();
        ui->lcd_primary->display(s);
        m_dPrimary = s.toDouble();

        s = result["primaryProbeOffset"].toString();
        ui->lcd_primary_offset->display(s);
        m_dRTD5_OffsetValue = s.toDouble();

        s = result["secondaryProbeTemp"].toString();
        ui->lcd_secondary->display(s);
        m_dSecondary = s.toDouble();

        s = result["controlProbeTemp"].toString();
        ui->lcd_control->display(s);
        m_dControl = s.toDouble();

        s = result["controlProbeOffset"].toString();
        ui->lcd_control_offset->display(s);
        m_dRTD4_OffsetValue = s.toDouble();

        s = result["compressorProbeTemp"].toString();
        ui->lcd_compressor->display(s);
        m_dCompressor = s.toDouble();

        s = result["deviceType"].toString();
        ui->device_type_label->setText(s);
        m_sDeviceType = s;

        s = result["acVolt"].toString();
        ui->lcd_ac->display(s);

        s = result["batteryVolt"].toString();
        ui->lcd_battery->display(s);

        s = result["productMaxTemp"].toString();
        ui->lcd_max->display(s);

        s = result["productMinTemp"].toString();
        ui->lcd_min->display(s);

        s = result["minMaxLastReset"].toString();
        ui->label_min_max_reset->setText(s + " mins");

        s = result["powerState"].toString();
        ui->label_power_state->setText(s);
        if(s.compare("ac")==0)
            ui->led_power_state->setPixmap(m_ledOFF);
        else
            ui->led_power_state->setPixmap(m_ledON);

        s = result["batteryState"].toString();
        ui->label_battery_state->setText(s);
        if(s.contains("good"))
            ui->led_battery_state->setPixmap(m_ledOFF);
        else
            ui->led_battery_state->setPixmap(m_ledON);

        s = result["doorStatus"].toString();
        ui->label_door_state->setText(s);
        if(s.compare("closed")==0)
            ui->led_door_state->setPixmap(m_ledOFF);
        else
            ui->led_door_state->setPixmap(m_ledON);

        s = result["peltierTestActive"].toString();
        ui->label_peltier_state->setText(s);
        if(s.compare("no")==0)
            ui->led_peltier_active->setPixmap(m_ledOFF);
        else
            ui->led_peltier_active->setPixmap(m_ledON);

        s = result["doorAlarmActive"].toString();
        ui->label_door_alarm->setText(s);
        if(s.compare("no")==0)
            ui->led_door_alarm->setPixmap(m_ledOFF);
        else
            ui->led_door_alarm->setPixmap(m_ledON);

        s = result["primaryProbeAlarmActive"].toString();
        ui->label_primary_alarm->setText(s);
        if(s.compare("normal")==0)
            ui->led_primary_probe->setPixmap(m_ledOFF);
        else
            ui->led_primary_probe->setPixmap(m_ledON);

        s = result["secondaryProbeAlarmActive"].toString();
        ui->label_secondary_alarm->setText(s);
        if(s.compare("normal")==0)
            ui->led_secondary_probe->setPixmap(m_ledOFF);
        else
            ui->led_secondary_probe->setPixmap(m_ledON);

        s = result["controlProbeAlarmActive"].toString();
        ui->label_control_alarm->setText(s);
        if(s.compare("normal")==0)
            ui->led_control_probe->setPixmap(m_ledOFF);
        else
            ui->led_control_probe->setPixmap(m_ledON);

        s = result["compressorProbeAlarmActive"].toString();
        ui->label_compressor_alarm->setText(s);
        if(s.compare("normal")==0)
            ui->led_compressor_probe->setPixmap(m_ledOFF);
        else
            ui->led_compressor_probe->setPixmap(m_ledON);

        s = result["compressorState"].toString();
        ui->label_compressor_state->setText(s);
        if(s.compare("off")==0)
            m_bCompressorState = false;
        else
            m_bCompressorState = true;

        s = result["lockState"].toString();
        ui->label_lock_state->setText(s);
        if(s.compare("locked")==0)
            ui->led_lock_state->setPixmap(m_ledOFF);
        else
            ui->led_lock_state->setPixmap(m_ledON);

        s = result["defrostStatus"].toString();
        ui->label_defrost_state->setText(s);
        if(s.compare("off")==0)
            ui->led_defrost_active->setPixmap(m_ledOFF);
        else
            ui->led_defrost_active->setPixmap(m_ledON);

        db.insertTransducerEntry(m_dCompressor,
                                 m_dSecondary,
                                 UNUSED_PROBE_VALUE,
                                 m_dControl,
                                 m_dPrimary);

        if ( m_pCalibrationManager != NULL )
        {
            eCalibrationStates eState = m_pCalibrationManager->getCalibrationState();
            if ( eState == eCALIBRATION_STATE_TEMPERATURE_UNSTABLE )
            {
                ui->label_calibration_state->setText("Unstable");
                ui->led_calibrated->setPixmap(m_ledON);
            }
            else if ( eState == eCALIBRATION_STATE_TEMPERATURE_STABLE )
            {
                ui->label_calibration_state->setText("Calibrating");
                ui->led_calibrated->setPixmap(m_ledON);
            }
            else if ( eState == eCALIBRATION_STATE_CALIBRATED )
            {
                ui->label_calibration_state->setText("Calibrated!");
                ui->led_calibrated->setPixmap(m_ledOFF);
            }
            else
            {
                ui->label_calibration_state->setText("---");
                ui->led_calibrated->setPixmap(m_ledON);
            }
        }

    }

  //}
  //else
  //{
  //    qDebug() << "Client::receiveMessage -> CANNOT READ LINE";
  //}

}

QString Client::getPayload( QString sData, QChar cStartOfPayload )
{
    QString sPayload;
    QString sTemp;

    sTemp = sData.section(cStartOfPayload, 1, -1 );

    if ( sTemp.length() > 0 )
    {
        sPayload = cStartOfPayload + sTemp;
    }

    return sPayload;
}

void Client::connectionClosed()
{
  ui->connectDisconnectButton->setText("Connect");
  ui->connectDisconnectButton->setEnabled(true);
  ui->inputLineEdit->setEnabled(false);
  ui->sendButton->setEnabled(false);
  ui->sendContButton->setEnabled(false);
  ui->stopButton->setEnabled(false);

  ui->button_control_down->setEnabled(false);
  ui->button_control_up->setEnabled(false);
  ui->button_light_off->setEnabled(false);
  ui->button_light_on->setEnabled(false);
  ui->button_lock->setEnabled(false);
  ui->button_unlock->setEnabled(false);
  ui->button_match_primary->setEnabled(false);
  ui->button_peltier_low->setEnabled(false);
  ui->button_peltier_high->setEnabled(false);
  ui->button_peltier_stop->setEnabled(false);
  ui->button_primary_down->setEnabled(false);
  ui->button_primary_up->setEnabled(false);
  ui->button_auto_cal->setEnabled(false);

  ui->led_power_state->setPixmap(m_ledOFF);
  ui->led_battery_state->setPixmap(m_ledOFF);
  ui->led_door_state->setPixmap(m_ledOFF);
  ui->led_peltier_active->setPixmap(m_ledOFF);
  ui->led_door_alarm->setPixmap(m_ledOFF);
  ui->led_primary_probe->setPixmap(m_ledOFF);
  ui->led_secondary_probe->setPixmap(m_ledOFF);
  ui->led_control_probe->setPixmap(m_ledOFF);
  ui->led_compressor_probe->setPixmap(m_ledOFF);
  ui->led_calibrated->setPixmap(m_ledOFF);

  ui->lcd_primary->display("---");
  ui->lcd_secondary->display("---");
  ui->lcd_control->display("---");
  ui->lcd_compressor->display("---");
  ui->lcd_ac->display("---");
  ui->lcd_battery->display("---");
  ui->lcd_max->display("---");
  ui->lcd_min->display("---");
  ui->lcd_primary_offset->display("---");
  ui->lcd_control_offset->display("---");
  ui->lcd_fluke_1->display("---");
  ui->lcd_fluke_2->display("---");

  sendMessageTimer->stop();
  flukeTimer.stop();

  if (conButtonClicked)
  {
      contDisconnects++;
  }

}

void Client::socketError()
{
  ui->chatDisplayTextEdit->setText(QString("Socket Er  flukeTimer.start(TIMEOUT_FLUKE_TEMP_UPDATE_SEC*1000);ror: %1").arg(socket.errorString()));
  if (socket.state() != QAbstractSocket::ConnectedState)
  {
    connectionClosed();
  }
  socket.close();
}

void Client::on_dial_valueChanged(int value)
{
    ui->dialLabel->setText(QString::number(value) + " ms");
    m_iCurrentDialValue = value;
}

void Client::on_sendContButton_clicked()
{
    sendMessageTimer->start(m_iCurrentDialValue);
    flukeTimer.start(TIMEOUT_FLUKE_TEMP_UPDATE_SEC*1000);

    if (!dataTimer.isActive())
    {
        connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
        dataTimer.start(TIMEOUT_GRAPH_UPDATE_SEC*1000);
    }
}

void Client::sendMessageTimeout()
{
    ui->inputLineEdit->clear();

    QString msg = m_sRequest_Status;

    ui->inputLineEdit->append(m_sRequest_Status);

    msgsSentCount++;

    socket.write(msg.toLocal8Bit().constData());

    ui->msgsSentLabel->setText(QString::number(msgsSentCount));

    ui->chatDisplayTextEdit->clear();
}

void Client::on_stopButton_clicked()
{
    sendMessageTimer->stop();
    flukeTimer.stop();
}

void Client::on_button_lock_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_Lock;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::on_button_unlock_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_Unlock;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::on_button_light_on_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_LightOn;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::on_button_light_off_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_LightOff;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::on_button_peltier_high_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_PeltierHigh;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::on_button_peltier_low_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_PeltierLow;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::on_button_peltier_stop_clicked()
{
    sendMessageTimer->stop();
    QString msg = m_sRequest_PeltierOff;
    socket.write(msg.toLocal8Bit().constData());
    msgsSentCount++;
    ui->chatDisplayTextEdit->clear();
    sendMessageTimer->start(m_iCurrentDialValue);
}

void Client::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    //static double lastPointKey = 0;
    //if (key-lastPointKey > 0.002) // at most add point every 2 ms
    //{
      // add data to lines:
//      ui->graph->graph(0)->addData(key, qSin(key)+qrand()/(double)RAND_MAX*1*qSin(key/0.3843));
//      ui->graph->graph(1)->addData(key, qCos(key)+qrand()/(double)RAND_MAX*0.5*qSin(key/0.4364));
        ui->graph->graph(0)->addData(key, m_dPrimary);
        ui->graph->graph(1)->addData(key, m_dSecondary);
        ui->graph->graph(2)->addData(key, m_dControl);
        ui->graph->graph(3)->addData(key, m_dCompressor);

        ui->graph_fluke->graph(0)->addData(key, m_dFlukeChannel1);
        ui->graph_fluke->graph(1)->addData(key, m_dFlukeChannel2);

      // rescale value (vertical) axis to fit the current data:
        ui->graph->graph(0)->rescaleValueAxis(true);
        ui->graph->graph(1)->rescaleValueAxis(true);
        ui->graph->graph(2)->rescaleValueAxis(true);
        ui->graph->graph(3)->rescaleValueAxis(true);

        ui->graph_fluke->graph(0)->rescaleValueAxis(true);
        ui->graph_fluke->graph(1)->rescaleValueAxis(true);
      //lastPointKey = key;
    //}
    // make key axis range scroll with the data (at a constant range size of 8):

//        const QString sformat="hh:mm:ss"; //Generate Date


    ui->graph->xAxis->setRange(key, GRAPH_X_AXIS_MINUTES*60, Qt::AlignRight);
    ui->graph_fluke->xAxis->setRange(key, GRAPH_X_AXIS_MINUTES*60, Qt::AlignRight);


    ui->graph->replot();
    ui->graph_fluke->replot();

}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::sendSerialRequest( QString sPortName, int iWaitTimeoutMS, QByteArray baRequest )
{
    m_SerialPort.transaction( sPortName, iWaitTimeoutMS, baRequest );
}

//-------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::handleResponse( QByteArray baResponse )
{
    m_bWaitingForTimeout = false;

//    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
//    qDebug() << "   Serial Port Response";
//    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    QString sResponse = QString(baResponse);
    sResponse.replace("\r", "");
    sResponse.replace("\n", "");
//    qDebug() << sResponse;
//    qDebug() << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";

    float fTemp;

    if( sResponse.contains("e+") )
    {
//        qDebug() << "HAS -> e+";
        fTemp = sResponse.toFloat();
    }
    else
    {
        qWarning() << "Serial Port Response Error: " << sResponse;
        if (m_eLastFlukeMsgSent == eFLUKE_TC_1)
        {
            ui->lcd_fluke_1->display(QString("---"));
        }
        else if (m_eLastFlukeMsgSent == eFLUKE_TC_2)
        {
            ui->lcd_fluke_2->display(QString("---"));
        }
        return;
    }

//    qDebug() << "TEMP: " << fTemp;

    if (m_eLastFlukeMsgSent == eFLUKE_TC_1)
    {
        m_dFlukeChannel1 = fTemp;
        ui->lcd_fluke_1->display(QString::number(fTemp,'f',1));
    }
    else if (m_eLastFlukeMsgSent == eFLUKE_TC_2)
    {
        m_dFlukeChannel2 = fTemp;
        ui->lcd_fluke_2->display(QString::number(fTemp,'f',1));
    }
}

//-------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::handleSerialPortError( QString sError )
{
    qWarning() << sError;
    qDebug() << "Serial Port Error: " << sError;
    m_bWaitingForTimeout = false;
}

//-------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::handleTimeout( QString sMessage )
{
    qDebug() << "Serial Port Timeout: " << sMessage;
    m_bWaitingForTimeout = false;

    ui->lcd_fluke_1->display("---");
    ui->lcd_fluke_2->display("---");

    // ---------------------------------------------------------
    disconnect(&m_SerialPort, SIGNAL(response(QByteArray)), this, SLOT(handleResponse(QByteArray)));
    disconnect(&m_SerialPort, SIGNAL(error(QString)), this, SLOT(handleSerialPortError(QString)));
    disconnect(&m_SerialPort, SIGNAL(timeout(QString)), this, SLOT(handleTimeout(QString)));

    m_bSerialPortFound = findSerialPort();

    connect(&m_SerialPort, SIGNAL(response(QByteArray)), this, SLOT(handleResponse(QByteArray)));
    connect(&m_SerialPort, SIGNAL(error(QString)), this, SLOT(handleSerialPortError(QString)));
    connect(&m_SerialPort, SIGNAL(timeout(QString)), this, SLOT(handleTimeout(QString)));


}


void Client::flukeTempTimeout( void )
{
    if ( m_eLastFlukeMsgSent == eFLUKE_TC_1 )
    {
        readFlukeTemp2();
    }
    else if ( m_eLastFlukeMsgSent == eFLUKE_TC_2 )
    {
        readFlukeTemp1();
    }
    else
    {
        readFlukeTemp1();
    }
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::readFlukeTemp1( void )
{
    m_eLastFlukeMsgSent = eFLUKE_TC_1;

    if ( m_bWaitingForTimeout == false )
    {
        QByteArray baFlukeTempReq;

        const char FLUKE_TEMP_1_COMMAND[]   = {"MEAS:TEMP? TC,T,(@101)\r\n"};
        baFlukeTempReq.clear();
        baFlukeTempReq.append( FLUKE_TEMP_1_COMMAND, strlen(FLUKE_TEMP_1_COMMAND) );
//        qDebug() << "===========================================================";
//        qDebug() << "  readFlukeTemp1 ";
//        qDebug() << "===========================================================";
//        qDebug() << baFlukeTempReq ;
//        qDebug() << "===========================================================";

        sendSerialRequest(m_sComPort, m_iWaitTimeoutMS, baFlukeTempReq );
        m_bWaitingForTimeout = true;
    }
    else
    {
        qWarning() << "Can not send the readFlukeTemp1 command - waiting for previous serial port command response";
    }
}
//-----------------------------------------------------------------------------------------------------------------
void Client::readFlukeTemp2( void )
{
    m_eLastFlukeMsgSent = eFLUKE_TC_2;

//    if ( m_bWaitingForTimeout == false )
//    {
//        QByteArray baFlukeTempReq;

//        const char FLUKE_TEMP_2_COMMAND[]   = {"MEAS:TEMP? TC,T,(@102)\r\n"};
//        baFlukeTempReq.clear();
//        baFlukeTempReq.append( FLUKE_TEMP_2_COMMAND, strlen(FLUKE_TEMP_2_COMMAND) );

//        sendSerialRequest(m_sComPort, m_iWaitTimeoutMS, baFlukeTempReq );
//        m_bWaitingForTimeout = true;
//    }
//    else
//    {
//        qWarning() << "Can not send the getFlukeTemp2 command - waiting for previous serial port command response";
//    }
}


//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::turnLightOn(void)
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
void Client::turnLightOff( void )
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
void Client::setSerialPortName( QString sSerialPortName )
{
    m_sComPort = sSerialPortName;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
void Client::setWaitTimeoutMS( int iTimeoutMS )
{
    m_iWaitTimeoutMS = iTimeoutMS;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
bool Client::findSerialPort( void )
{
    bool bRC = false;
    QSerialPort serial;
    QString sPortName;
    QSerialPortInfo info;

    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    QListIterator<QSerialPortInfo> i(infos);
    while (i.hasNext())
    {
        info = i.next();
        sPortName = info.portName();
//        qDebug() << "Trying Serial Port: " << sPortName;
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
//            qDebug() << "Serial Port: " << sPortName << " not connected: " << serial.errorString();
        }
    }

    serial.close();

    return bRC;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
bool Client::isSerialPortFound( void )
{
    return m_bSerialPortFound;
}

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
QString Client::byteArrayToHexString( QByteArray & buffer )
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

void Client::sendCalibrationRequest(eRTDNumber eRTDNum , double dRTDOffsetVal)
{
    QString sOffset = QString::number(dRTDOffsetVal,'f',1);


    QString sJSON = "{\n";
    if ( eRTDNum == eRTD4 )
    {
        sJSON.append("\"RTD4Offset\":\"");
    }
    else if ( eRTDNum == eRTD5 )
    {
        sJSON.append("\"RTD5Offset\":\"");
    }
    else
    {
        qDebug() << "Client::packageCalibrationRequest - INVALID";
        return;
    }
    sJSON.append(sOffset);
    sJSON.append("\"\n}");


    QString sJSONLength = QString::number(sJSON.length());

    QString sFullMsg;
    sFullMsg.append("PUT /eqc/v1/calibration HTTP/1.1\n");
    sFullMsg.append("Authorization: Basic SGVsbWVyOkFwcGxlMTIzNDU=\n");
    sFullMsg.append("Host: 192.168.0.3:5090\n");
    sFullMsg.append("Content-Length: ");
    sFullMsg.append(sJSONLength);
    sFullMsg.append("\n\n");
    sFullMsg.append(sJSON);

    qDebug() << "-----------------------------";
    qDebug() << sFullMsg;
    qDebug() << "-----------------------------";

    msgsSentCount++;
    socket.write(sFullMsg.toLocal8Bit().constData());
}

void Client::on_button_match_primary_clicked()
{

    // (REFERENCE READING [CHANNEL 1]) - (IC3 READING [PRIMARY PROBE]) = 'OFFSET_DELTA'
    double fOffsetDelta = m_dFlukeChannel1 - m_dPrimary;

    // VALIDATE VALUE +/- 10.0
    if ( fOffsetDelta <= -10.0 ||
         fOffsetDelta >= 10.0 )
    {
        return;
    }
    m_dRTD5_OffsetValue = m_dRTD5_OffsetValue + fOffsetDelta;

    // SEND 'OFFSET_DELTA' VALUE INTO CALIBRATION REQUEST (FOR RTD5)
    sendCalibrationRequest(eRTD5, m_dRTD5_OffsetValue);

    // UPDATE OFFSET VALUE ON GUI
    ui->lcd_primary_offset->display(QString::number(m_dRTD5_OffsetValue,'f',1));

}

void Client::on_button_primary_up_clicked()
{
    // VALIDATE VALUE +/- 10.0
    if ( m_dRTD5_OffsetValue <= -10.0 ||
         m_dRTD5_OffsetValue >= 10.0 )
    {
        return;
    }

    // INCREMENT CURRENT RTD5 OFFSET VALUE
    m_dRTD5_OffsetValue = m_dRTD5_OffsetValue + 0.1;

    // SEND OFFSET VALUE INTO CALIBRATION REQUEST (FOR RTD5)
    sendCalibrationRequest(eRTD5, m_dRTD5_OffsetValue);

    // UPDATE OFFSET VALUE ON GUI
    ui->lcd_primary_offset->display(QString::number(m_dRTD5_OffsetValue,'f',1));
}

void Client::on_button_primary_down_clicked()
{
    // VALIDATE VALUE +/- 10.0
    if ( m_dRTD5_OffsetValue <= -10.0 ||
         m_dRTD5_OffsetValue >= 10.0 )
    {
        return;
    }

    // DECREMENT CURRENT RTD5 OFFSET VALUE
    m_dRTD5_OffsetValue = m_dRTD5_OffsetValue - 0.1;

    // SEND OFFSET VALUE INTO CALIBRATION REQUEST (FOR RTD5)
    sendCalibrationRequest(eRTD5, m_dRTD5_OffsetValue);

    // UPDATE OFFSET VALUE ON GUI
    ui->lcd_primary_offset->display(QString::number(m_dRTD5_OffsetValue,'f',1));
}

void Client::on_button_control_up_clicked()
{
    // VALIDATE VALUE +/- 10.0
    if ( m_dRTD4_OffsetValue <= -10.0 ||
         m_dRTD4_OffsetValue >= 10.0 )
    {
        return;
    }

    // INCREMENT CURRENT RTD4 OFFSET VALUE
    m_dRTD4_OffsetValue = m_dRTD4_OffsetValue + 0.1;

    // SEND OFFSET VALUE INTO CALIBRATION REQUEST (FOR RTD4)
    sendCalibrationRequest(eRTD4, m_dRTD4_OffsetValue);

    // UPDATE OFFSET VALUE ON GUI
    ui->lcd_control_offset->display(QString::number(m_dRTD4_OffsetValue,'f',1));
}

void Client::on_button_control_down_clicked()
{
    // VALIDATE VALUE +/- 10.0
    if ( m_dRTD4_OffsetValue <= -10.0 ||
         m_dRTD4_OffsetValue >= 10.0 )
    {
        return;
    }

    // DECREMENT CURRENT RTD4 OFFSET VALUE
    m_dRTD4_OffsetValue = m_dRTD4_OffsetValue - 0.1;

    // SEND OFFSET VALUE INTO CALIBRATION REQUEST (FOR RTD4)
    sendCalibrationRequest(eRTD4, m_dRTD4_OffsetValue);

    // UPDATE OFFSET VALUE ON GUI
    ui->lcd_control_offset->display(QString::number(m_dRTD4_OffsetValue,'f',1));
}

double Client::getFlukeTemp1()
{
    return m_dFlukeChannel1;
}

double Client::getPrimaryTemp()
{
    return m_dPrimary;
}

double Client::getControlTemp()
{
    return m_dControl;
}

double Client::getPrimaryOffset()
{
    return m_dRTD5_OffsetValue;
}

double Client::getControlOffset()
{
    return m_dRTD4_OffsetValue;
}

bool Client::getCompressorState()
{
    return m_bCompressorState;
}

QString Client::getDeviceType()
{
    return m_sDeviceType;
}

void Client::on_button_auto_cal_clicked()
{
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    qDebug() << "AUTOMATIC CALIBRATION STARTED!!!";
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    m_pCalibrationManager = new CalibrationManager(this);
    ui->button_auto_cal->setText("In Progress..");
    ui->button_control_down->setEnabled(false);
    ui->button_control_up->setEnabled(false);
    ui->button_light_off->setEnabled(false);
    ui->button_light_on->setEnabled(false);
    ui->button_lock->setEnabled(false);
    ui->button_unlock->setEnabled(false);
    ui->button_match_primary->setEnabled(false);
    ui->button_peltier_low->setEnabled(false);
    ui->button_peltier_high->setEnabled(false);
    ui->button_peltier_stop->setEnabled(false);
    ui->button_primary_down->setEnabled(false);
    ui->button_primary_up->setEnabled(false);
    ui->button_auto_cal->setEnabled(false);
}
