#include <QDateTime>
#include <QMessageBox>
#include "client.h"
#include "ui_client.h"
#include <QList>
#include "qcustomplot.h"


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
    m_dCompressor(0.0)
{
  ui->setupUi(this);

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
  connDisTimer = new QTimer(this);
  connect(connDisTimer, SIGNAL(timeout()), this, SLOT(connDisTimeout()));

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

  ui->led_power_state->setPixmap(m_ledOFF);
  ui->led_battery_state->setPixmap(m_ledOFF);
  ui->led_door_state->setPixmap(m_ledOFF);
  ui->led_peltier_active->setPixmap(m_ledOFF);
  ui->led_door_alarm->setPixmap(m_ledOFF);
  ui->led_primary_probe->setPixmap(m_ledOFF);
  ui->led_secondary_probe->setPixmap(m_ledOFF);
  ui->led_control_probe->setPixmap(m_ledOFF);
  ui->led_compressor_probe->setPixmap(m_ledOFF);

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

  // create and prepare a text layout element:
  QCPTextElement *legendTitle = new QCPTextElement(ui->graph);
  legendTitle->setLayer(ui->graph->legend->layer()); // place text element on same layer as legend, or it ends up below legend
  legendTitle->setText("Engine Status");
  legendTitle->setFont(QFont("sans", 9, QFont::Bold));


  QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  timeTicker->setTimeFormat("%h:%m:%s");
  ui->graph->xAxis->setTicker(timeTicker);
  ui->graph->axisRect()->setupFullAxesBox();
  ui->graph->yAxis->setRange(0.0, 10.0);


  // make left and bottom axes transfer their ranges to right and top axes:
  connect(ui->graph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph->xAxis2, SLOT(setRange(QCPRange)));
  connect(ui->graph->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->graph->yAxis2, SLOT(setRange(QCPRange)));

  db.openDatabase();

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

        s = result["secondaryProbeTemp"].toString();
        ui->lcd_secondary->display(s);
        m_dSecondary = s.toDouble();

        s = result["controlProbeTemp"].toString();
        ui->lcd_control->display(s);
        m_dControl = s.toDouble();

        s = result["compressorProbeTemp"].toString();
        ui->lcd_compressor->display(s);
        m_dCompressor = s.toDouble();

        s = result["deviceType"].toString();
        ui->device_type_label->setText(s);

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

        db.insertTransducerEntry(m_dCompressor,
                                 m_dSecondary,
                                 UNUSED_PROBE_VALUE,
                                 m_dControl,
                                 m_dPrimary);
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
  sendMessageTimer->stop();

  if (conButtonClicked)
  {
      contDisconnects++;
  }

}

void Client::socketError()
{
  ui->chatDisplayTextEdit->setText(QString("Socket Error: %1").arg(socket.errorString()));
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
}


void Client::connDisTimeout()
{
    if (socket.state() == QAbstractSocket::UnconnectedState)
    {
      // Initiate an SSL connection to the chat server.
      socket.connectToHostEncrypted(ui->hostnameLineEdit->text(), ui->portSpinBox->value());
      //contConnects++;
      //ui->numConLabel->setText(QString::number(contConnects));
    }

    else
    {
      socket.close();
      //contDisconnects++;
      //ui->numDisLabel->setText(QString::number(contDisconnects));
    }
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

      // rescale value (vertical) axis to fit the current data:
        ui->graph->graph(0)->rescaleValueAxis(true);
        ui->graph->graph(1)->rescaleValueAxis(true);
        ui->graph->graph(2)->rescaleValueAxis(true);
        ui->graph->graph(3)->rescaleValueAxis(true);
      //lastPointKey = key;
    //}
    // make key axis range scroll with the data (at a constant range size of 8):

//        const QString sformat="hh:mm:ss"; //Generate Date


    ui->graph->xAxis->setRange(key, GRAPH_X_AXIS_MINUTES*60, Qt::AlignRight);


    ui->graph->replot();




    // calculate frames per second:
//    static double lastFpsKey;
//    static int frameCount;
//    ++frameCount;
//    if (key-lastFpsKey > 2) // average fps over 2 seconds
//    {
//      ui->statusBar->showMessage(
//            QString("%1 FPS, Total Data points: %2")
//            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
//            .arg(ui->graph->graph(0)->data()->size()+ui->graph->graph(1)->data()->size())
//            , 0);
//      lastFpsKey = key;
//      frameCount = 0;
//    }
}

