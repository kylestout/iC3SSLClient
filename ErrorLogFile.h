#ifndef IC3_ERRORLOGFILE_H
#define IC3_ERRORLOGFILE_H

/**
*     @file ErrorLogFile.h
*     @author Doug Sanqunetti
*     @date 7/17/2016
*     @version 1.0
*     @brief This cpp file defines the ErrorLogFile class.  This class is used to log error messages
*            to a log file.
*
*/

#include <QObject>

class ErrorLogFile : public QObject
{
    Q_OBJECT

public:
    explicit ErrorLogFile(QObject *parent = 0);

    void setLogFileName( QString sFileName );
    void setMaxLogFileSize( int iMaxLogFileSize );
    int  getMaxLogFileSize( void );

    bool logErrorMessage(QString sErrorMessage );

private:

    int     m_iMaxLogFileSize;
    QString m_sFileName;

};

#endif // IC3_ERRORLOGFILE_H
