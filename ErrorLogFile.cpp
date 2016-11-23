/**
*     @file ErrorLogFile.cpp
*     @author Doug Sanqunetti
*     @date 7/17/2016
*     @version 1.0
*     @brief This cpp file implements the ErrorLogFile class.  This class is used to log error messages
*            to a log file.
*
*/

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDataStream>
#include "ErrorLogFile.h"

ErrorLogFile::ErrorLogFile(QObject *parent) :
    QObject(parent),
    m_iMaxLogFileSize( 500000 ),
    m_sFileName( "./logFile.txt" )
{
}

//--------------------------------------------------------------------------------------
/** setMaxLogFileSize() - Mutator function to set the maximum log file size.  If this
*                         function is not called, the default value is used (30,000 bytes)
*  @retval - the maximum log file size
*  @author Doug Sanqunetti
*  @date 5/17/2015
*/
//--------------------------------------------------------------------------------------
void ErrorLogFile::setMaxLogFileSize( int iMaxLogFileSize )
{
    m_iMaxLogFileSize = iMaxLogFileSize;
}

//--------------------------------------------------------------------------------------
/** getMaxLogFileSize() - accessor function to retrieve the maximum log file size
*  @retval - the maximum log file size
*  @author Doug Sanqunetti
*  @date 5/17/2015
*/
//--------------------------------------------------------------------------------------
int  ErrorLogFile::getMaxLogFileSize( void )
{
    return m_iMaxLogFileSize;
}

//--------------------------------------------------------------------------------------
/** logErrorMessage() - function that logs error messages and ensures the error log
*                       does not exceed the maximum size
*  @retval - none
*  @author Doug Sanqunetti
*  @date 5/17/2015
*/
//--------------------------------------------------------------------------------------
bool ErrorLogFile::logErrorMessage( QString sErrorMessage )
{
    QString sBackupPath;

    QFile file(m_sFileName);

    if( !file.open(QFile::ReadWrite | QFile::Append | QFile::Text))
    {
        return false;
    }

    if ( file.size() >= m_iMaxLogFileSize )
    {
        file.close();

        sBackupPath = QString( "%1.bak").arg( m_sFileName );

        // only keep one backup so remove old backup if one exists
        if (QFile::exists(sBackupPath))
        {
            QFile::remove(sBackupPath);
        }

        // backup current logfile
        QFile::rename(m_sFileName, sBackupPath);


        // now re-open the file
        if( !file.open(QFile::ReadWrite | QFile::Append | QFile::Text))
        {
            return false;
        }
    }

    QTextStream OutStream(&file);

    OutStream << sErrorMessage;
    OutStream.flush();
    file.flush();
    file.close();

    return true;
}

//--------------------------------------------------------------------------------------
/** setLogFileName() - set the error log file name
*  @retval - none
*  @author Doug Sanqunetti
*  @date 5/17/2015
*/
//--------------------------------------------------------------------------------------
void ErrorLogFile::setLogFileName( QString sFileName )
{
    m_sFileName = sFileName;
}
