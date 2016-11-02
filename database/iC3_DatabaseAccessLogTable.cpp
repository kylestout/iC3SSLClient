/**
*     @file iC3_DatabaseAccessLogTable.cpp
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief This cpp file implements the iC3_DatabaseAccessLogTable class.  This class is
*            responsible for creating, reading from and writing to the Access Log Table in
*            the database.
*/

#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include "iC3_DatabaseAccessLogTable.h"

//-----------------------------------------------------------------------------------------------
/** constructor - defines the columns in the Access Log Table
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DatabaseAccessLogTable::iC3_DatabaseAccessLogTable()
{
    //--------------------
    // TINYINT =  1-Byte
    // INTEGER =  4-Bytes
    // BIGINT  =  8-Bytes
    //--------------------

    iC3_DatabaseColumnDef * pColumn;

    m_sTableName = "accessControlLog";

    setNumberOfColumns( e_NUMBER_OF_ACCESS_LOG_TABLE_COLUMNS );

    pColumn = new iC3_DatabaseColumnDef( tr("accessEvent"), "INTEGER", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_ACCESSS_EVENT_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("userName"), "TEXT", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_USER_NAME_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("dateTime"), "DATETIME", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_DATE_TIME_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("accessMethod"), "INTEGER", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_ACCESS_METHOD_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("accessDuration"), "INTEGER", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_ACCESS_DURATION_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("viewed"), "BOOLEAN", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_VIEWED_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("downloaded"), "BOOLEAN", "" );
    AddColumnDef( e_ACCESS_LOG_TABLE_DOWNLOADED_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("eventSequenceIndex"), "BIGINT UNSIGNED", "PRIMARY KEY" );  //64 bit
    AddColumnDef( e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL, pColumn );
}

//-----------------------------------------------------------------------------------------------
/** getTableCreationSQL() - retrieves the table creation SQL statement.  The returned SQL
*                           statement is based upon the columns defined in the constructor
*   @retval Table creation SQL (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseAccessLogTable::getTableCreationSQL( void )
{
    return iC3_DatabaseTable::getTableCreationSQL( m_sTableName );
}

//-----------------------------------------------------------------------------------------------
/** CreateTable() - Creates the Access Log database table.
*   @param database - a reference to the QSqlDatabase object where this table will be created.
*                     The database must already be open.
*   @retval true - The Access Log database table was created.
*   @retval false - The Access Log table was not created.  Use iC3_DatabaseTable::GetLastError()
*                   to retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::CreateTable( QSqlDatabase & database )
{
    bool bRC = false;
    QString sCreationSQL;
    QSqlQuery query( database );

    ClearLastError();

    if ( database.isOpen() )
    {
        sCreationSQL = getTableCreationSQL();
        qDebug() << sCreationSQL;
        bRC = query.exec( sCreationSQL );

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_DatabaseAccessLogTable::CreateTable() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }
    }
    else
    {
        QString sErrorMessage = "iC3_DatabaseAccessLogTable::CreateTable() - Database is not open";
        SetLastError( sErrorMessage );
        qDebug() << m_sLastError;
        return false;
    }

    return bRC;
}

//-----------------------------------------------------------------------------------------------
/** getSQL_ColumnNames() - returns the Access Log Table column names in a form usable in an
*                          sql statement.
*   @retval Access Log Table column names in a form usable in an sql statement (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseAccessLogTable::getSQL_ColumnNames( void )
{
    QString sColumnNames;

    int iIndex;

    sColumnNames = QString("( %1").arg(getColumnDef(0)->getColumnName());

    for (iIndex = 1; iIndex < e_NUMBER_OF_ACCESS_LOG_TABLE_COLUMNS; iIndex++ )
    {
        sColumnNames.append(QString(", %1").arg(getColumnDef(iIndex)->getColumnName()));
    }
    sColumnNames.append(" )");

    return sColumnNames;
}

//-----------------------------------------------------------------------------------------------
/** getSQL_ColumnValues() - returns the Access Log Table column values in a form usable in an
*                          sql insert statement.
*   @param pAccessControlEntry - Pointer to an iC3_AccessLogData object containing the column
*                                data.
*   @retval Access Log Table column values in a form usable in an sql statement (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseAccessLogTable::getSQL_ColumnValues(  iC3_AccessLogData *pAccessLogEntry )
{
    QString sValueString;

    QDateTime dtAccessDateTime = QDateTime( pAccessLogEntry->getAccessDate(),
                                            pAccessLogEntry->getAccessTime(),
                                            Qt::LocalTime );

    sValueString = QString("VALUES ( ");
    sValueString.append(QString("%1, ").arg( pAccessLogEntry->getAccessEvent() ));
    sValueString.append(QString("%1, ").arg( SQL_FormatString(pAccessLogEntry->getUser())));
    sValueString.append(QString("%1, ").arg( SQL_FormatQDateTime(dtAccessDateTime)));
    sValueString.append(QString("%1, ").arg( pAccessLogEntry->getAccessMethodEnum()));
    sValueString.append(QString("%1, ").arg( pAccessLogEntry->getIntDurationMinutes()));
    sValueString.append(QString("%1, ").arg( SQL_FormatBoolean(pAccessLogEntry->getAccessEventViewed())));
    sValueString.append(QString("%1, ").arg( SQL_FormatBoolean(pAccessLogEntry->getAccessEventDownloaded())));
    sValueString.append(QString("%1 )").arg( pAccessLogEntry->getEventSequenceIndex()));

    return sValueString;
}

//-----------------------------------------------------------------------------------------------
/** updateAccessLogDataFromQuery() - updates an iC3_AccessLogData object with the column
*                                values returned in a QSqlQuery.
*   @param pAccessLogEntry - Pointer to an iC3_AccessLogData object containing the column
*                                data.
*   @param query - Reference to a QSqlQuery containing the results of an executed query
*   @retval true - if the query is valid and pAccessLogEntry != NULL
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::updateAccessLogDataFromQuery( iC3_AccessLogData * pAccessLogEntry, QSqlQuery & query )
{   
    QDateTime accessDateTime;

    if (    ( query.isValid() )
         && ( pAccessLogEntry != NULL )
       )
    {
        pAccessLogEntry->setAccessEvent(query.value(e_ACCESS_LOG_TABLE_ACCESSS_EVENT_COL).toInt());
        pAccessLogEntry->setUser( query.value(e_ACCESS_LOG_TABLE_USER_NAME_COL).toString());
        accessDateTime = query.value(e_ACCESS_LOG_TABLE_DATE_TIME_COL).toDateTime();
        pAccessLogEntry->setAccessDate(accessDateTime.date());
        pAccessLogEntry->setAccessTime(accessDateTime.time());
        pAccessLogEntry->setAccessMethod(query.value( e_ACCESS_LOG_TABLE_ACCESS_METHOD_COL).toInt());
        pAccessLogEntry->setDurationMinutes(query.value(e_ACCESS_LOG_TABLE_ACCESS_DURATION_COL).toInt());
        pAccessLogEntry->setAccessEventViewed(query.value( e_ACCESS_LOG_TABLE_VIEWED_COL).toBool());
        pAccessLogEntry->setAccessEventDownloaded(query.value(e_ACCESS_LOG_TABLE_DOWNLOADED_COL).toBool());
        pAccessLogEntry->setEventSequenceIndex(query.value(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL).toLongLong());
        return true;
    }
    else
    {
        m_sLastError = "iC3_DatabaseAccessLogTable::updateAccessControlDataFromQuery():  query.isValid() returned false";
        return false;
    }
}

//-----------------------------------------------------------------------------------------------
/** insertNewEntry() - inserts a new Access Log entry into the Access Log table.
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param pEntry - Pointer to an iC3_AccessLogData object containing the data to be inserted
*   @retval true - if the data was successfully inserted
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::insertNewEntry(QSqlDatabase &database, iC3_AccessLogData *pEntry )
{
    QString queryString;
    QSqlQuery query( database );
    bool bRC = false;

    ClearLastError();

    if ( database.isOpen() )
    {
        queryString = QString( "INSERT INTO %1 ").arg(m_sTableName);
        queryString.append( getSQL_ColumnNames());
        queryString.append( QString(" %1").arg(getSQL_ColumnValues( pEntry )));

        qDebug() << queryString;
        bRC = query.exec(queryString);

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_DatabaseAccessLogTable::InsertNewEntry() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }
    }
    else
    {
        QString sErrorMessage = "iC3_DatabaseAccessLogTable::InsertNewEntry() - Database is not open";
        SetLastError( sErrorMessage );
        qDebug() << m_sLastError;
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
/** getAccessLogEntry() - retrieves an Access Log entry from the Access Log table.
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param ulEventSequenceIndex - Event Sequence Index of the Log Entry to be retrieved
*   @param pAccessLogEntry - pointer to an iC3_AccessLogData object where the Access
*                      Log entry will be stored.
*   @retval true - if the data was successfully retrieved
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::getAccessLogEntry( QSqlDatabase & database,
                                                    quint32 ulEventSequenceIndex,
                                                    iC3_AccessLogData * pAccessLogEntry )
{
    bool bRC = false;
    QSqlQuery query( database );
    QString queryString;

    if ( database.isOpen() )
    {
        queryString = QString( "SELECT * FROM %1 WHERE %2 = %3").arg(m_sTableName).arg(getColumnDef(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName()).arg(ulEventSequenceIndex);
        qDebug() << queryString;
        bRC = query.exec(queryString);

         if ( bRC == false )
         {
             QString sQueryError = query.lastError().text();
             SetLastError( QString("iC3_DatabaseAccessLogTable::getAccessLogEntry() - Query Error: %1").arg(sQueryError));
             qDebug() << m_sLastError;
             return false;
         }

         if( query.next() == true )
         {
             bRC = true;
             updateAccessLogDataFromQuery( pAccessLogEntry, query );
         }
         else
         {
             bRC = false;
         }
     }
     else
     {
         QString sErrorMessage = "iC3_DatabaseAccessLogTable::getAccessLogEntry() - Database is not open";
         SetLastError( sErrorMessage );
         qDebug() << m_sLastError;
         return false;
     }

    return bRC;
}

//-----------------------------------------------------------------------------------------------
/** getLastMultAccessLogData() - retrieves the last n Access Log entries from the Access Log table.
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param uiNumberOfEntries - the number of access log entries to retrieve
*   @param pReturnData - pointer to an iC3_DatabaseAccessLogData object where the Access
*                      Log entries will be stored.
*   @retval true - if the data was successfully retrieved
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::getLastMultAccessLogData( QSqlDatabase &database,
                                                           unsigned int uiNumberOfEntries,
                                                           iC3_DatabaseAccessLogData * pReturnData )
{
    bool bRC = false;

    iC3_AccessLogData * pAccessLogEntry;
    QString sColumnName;
    QString queryString;
    QSqlQuery query( database );

    ClearLastError();

    sColumnName = m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName();

    if ( database.isOpen() )
    {
        queryString = QString( "SELECT * FROM %1 ORDER BY %2 ASC LIMIT ((Select count([%2]) from [%1])-%3),%3").arg(m_sTableName).arg(sColumnName).arg(uiNumberOfEntries);
        qDebug() << queryString;
        bRC = query.exec(queryString);

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_DatabaseAccessLogTable::getLastMultAccessLogData() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }

        while ( query.next() )
        {
            pAccessLogEntry = new iC3_AccessLogData();

            if( pAccessLogEntry == NULL )
            {
                SetLastError(QString("iC3_DatabaseAccessLogTable::getLastMultAccessLogData()-unable to create a new iC3_AccessLogData object"));
                return false;
            }

            if ( updateAccessLogDataFromQuery( pAccessLogEntry, query ) == true )
            {
                pReturnData->AddAccessLogEntry(pAccessLogEntry);
            }
            else
            {
                return false;
            }

        }
    }
    else
    {
        QString sErrorMessage = "iC3_DatabaseAccessLogTable::getLastMultiEvents() - Database is not open";
        SetLastError( sErrorMessage );
        qDebug() << m_sLastError;
    }

    return bRC;
}

//-----------------------------------------------------------------------------------------------
/** updateAccessLogEntry() - updates an Access Log entry in the Access Control table.
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param pAccessLogEntry - pointer to an iC3_AccessLogData object containing the
*                     Access Log entry to be updated.
*   @retval true - if the data was successfully updated
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::updateAccessLogEntry( QSqlDatabase & database,
                                                       iC3_AccessLogData *pAccessLogEntry )
{
    bool bRC = false;
    QSqlQuery query( database );
    QString queryString;

    if ( database.isOpen() )
    {
        quint32 ulEventSequenceIndex =  pAccessLogEntry->getEventSequenceIndex();

        QDateTime dtAccessDateTime = QDateTime( pAccessLogEntry->getAccessDate(),
                                                pAccessLogEntry->getAccessTime(),
                                                Qt::LocalTime );

        queryString = QString( "UPDATE %1 SET ").arg(m_sTableName);

        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_ACCESSS_EVENT_COL)->getColumnName()).arg(pAccessLogEntry->getAccessEvent()));
        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_USER_NAME_COL)->getColumnName()).arg(SQL_FormatString(pAccessLogEntry->getUser())));
        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_DATE_TIME_COL)->getColumnName()).arg(SQL_FormatQDateTime(dtAccessDateTime)));
        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_ACCESS_METHOD_COL)->getColumnName()).arg(pAccessLogEntry->getAccessMethodEnum()));
        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_ACCESS_DURATION_COL)->getColumnName()).arg(pAccessLogEntry->getIntDurationMinutes()));
        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_VIEWED_COL)->getColumnName()).arg(SQL_FormatBoolean(pAccessLogEntry->getAccessEventViewed())));
        queryString.append( QString("%1 = %2 ").arg(getColumnDef(e_ACCESS_LOG_TABLE_DOWNLOADED_COL)->getColumnName()).arg(SQL_FormatBoolean(pAccessLogEntry->getAccessEventDownloaded())));

        queryString.append( QString("WHERE %1 = %2").arg(m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName()).arg(ulEventSequenceIndex));

        qDebug() << queryString;
        bRC = query.exec(queryString);

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_DatabaseAccessLogTable::updateAccessLogEntry() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }
    }
    else
    {
         QString sErrorMessage = "iC3_DatabaseAccessLogTable::updateAccessLogEntry() - Database is not open";
         SetLastError( sErrorMessage );
         qDebug() << m_sLastError;
         return false;
    }

    return bRC;
}

//-----------------------------------------------------------------------------------------------
/** SetEventsDownloaded() - sets the download flag to true for events between the begining
*                     event sequence index and the end sequence index
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param ulBeginSequenceIndex - The first ESIN in the range of events that were downloaded
*   @param ulEndSequenceIndex - the last ESIN in the range of events that were downloaded
*   @retval true - if the data was successfully updated
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 4/20/2014
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::setEventsDownloaded( QSqlDatabase & database,
                                                      quint32 ulBeginSequenceIndex,
                                                      quint32 ulEndSequenceIndex )
{
    bool bRC = true;
    QString queryString;
    QString sESINColumnName = m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName();
    QString sDownloadedColumnName =  m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_DOWNLOADED_COL)->getColumnName();
    QSqlQuery query( database );

    ClearLastError();

    queryString = QString( "UPDATE %1 SET %2 = 1 WHERE %3 >= %4 AND %3 <= %5 ").arg(m_sTableName).arg(sDownloadedColumnName).arg(sESINColumnName).arg(ulBeginSequenceIndex).arg(ulEndSequenceIndex);
    qDebug() << queryString;
    bRC = query.exec(queryString);
    if ( bRC == false )
    {
        QString sQueryError = query.lastError().text();
        SetLastError( QString("iC3_DatabaseAccessLogTable::SetEventsDownloaded() - Update Query Error: %1").arg(sQueryError));
        qDebug() << m_sLastError;
        return false;
    }

    return bRC;
}

//-----------------------------------------------------------------------------------------------
/** updateViewedEvents() - updates Access Log Entries to indicate they have been viewed
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param ulBeginESIN - first ESIN in the range of events to update
*   @param ulEndESIN - the last ESIN in the range of events to update
*   @retval true - if the data was successfully updated
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 05/30/2014
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::updateViewedEvents( QSqlDatabase & database, quint32 ulBeginESIN, quint32 ulEndESIN )
{
    bool bRC;
    QString queryString;
    QSqlQuery query( database );

    QString sViewedColumnName = m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_VIEWED_COL)->getColumnName();
    QString sESIN_ColumnName = m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName();

    queryString = QString( "UPDATE %1 SET %2 = 1 WHERE %3 >= %4 AND %3 <= %5").arg(m_sTableName).arg(sViewedColumnName).arg(sESIN_ColumnName).arg(ulBeginESIN).arg(ulEndESIN);

    qDebug() << queryString;
    bRC = query.exec(queryString);

    if ( bRC == false )
    {
        QString sQueryError = query.lastError().text();
        SetLastError( QString("iC3_DatabaseAccessLogTable::updateViewedEvents() - Update Query Error: %1").arg(sQueryError));
        qDebug() << m_sLastError;
    }

    return bRC;
}
//-----------------------------------------------------------------------------------------------
/** write_CSV_File() - Generates a CSV file containing the contents of the Access Log Table
*   @param database - a reference to the QSqlDatabase object where the table exists.
*                     The database must already be open.
*   @param pRequest - pointer to an iC3_DatabaseRequest object containing the
*                     begin and end date of the requested data
*   @param dateFormat - enumerated value indicating how dates should appear in the CSV file
*   @param timeFormat - enumerated value indicating how times should appear in the CSV file
*   @retval true - if the data was successfully written to the CSV file
*   @retval false - an error occurred.  Use iC3_DatabaseTable::GetLastError() to
*                   retrieve error information.
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseAccessLogTable::write_CSV_File( QSqlDatabase &database,
                                                 iC3_DatabaseRequest * pRequest,
                                                 eDateFormats dateFormat,
                                                 eTimeFormats timeFormat )
{
    bool bRC = false;
    QSqlQuery query( database );
    QString sWriteBuffer;
    QString queryString;
    quint32 ulBeginESIN = pRequest->getBeginSequenceIndex();
    quint32 ulEndESIN = pRequest->getEndSequenceIndex();

    QString sCSVFileName;
    QString sDateTime;
    QString sDate;
    QString sTime;
    QString sTempString;
    QDateTime dtRequestDateTime;
    ClearLastError();

    QFile  CSVFile;

    dtRequestDateTime = pRequest->getDateTime();
    sDate = dtRequestDateTime.toString(iC3_DMM_UtilityFunctions::convertToFilePathDateFormatString(dateFormat));
    sTime = dtRequestDateTime.toString(iC3_DMM_UtilityFunctions::convertToFilePathTimeFormatString(timeFormat));
    sDateTime = QString("%1_%2").arg(sDate).arg(sTime);

    sCSVFileName = QString("%1%2_%3_%4.csv").arg(CSV_FILE_PATH).arg(pRequest->getUnitID()).arg(tr("AccessLog")).arg(sDateTime);
    CSVFile.setFileName ( sCSVFileName );

    if ( CSVFile.open( QIODevice::WriteOnly ) == false )
    {
        QString szErrorMessage = QString("iC3_DatabaseAccessLogTable::Write_CSV_File() - Could not open a CSV file for writing: %1").arg(sCSVFileName);
        SetLastError( szErrorMessage );
        return false;
    }

    // write the file header
    sWriteBuffer = get_CSV_FileColumnHeaders();
    sWriteBuffer = sWriteBuffer.append("\r\n");
    CSVFile.write(sWriteBuffer.toAscii(), sWriteBuffer.length());
    qDebug() << sWriteBuffer;

    if ( database.isOpen() )
    {
        QString sESINColumnName = getColumnDef(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName();
        QString sBeginESIN = QString::number( ulBeginESIN );
        QString sEndESIN = QString::number( ulEndESIN );

        queryString = QString( "SELECT * FROM %1 WHERE %2 >=%3 AND %2 <= %4").arg(m_sTableName).arg(sESINColumnName).arg(sBeginESIN).arg(sEndESIN);
        qDebug() << queryString;
        bRC = query.exec(queryString);

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_DatabaseAccessLogTable::write_CSV_File() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }
        else
        {
            while ( query.next() )
            {
                sWriteBuffer = query.value(0).toString();

                for ( int iIndex = 1; iIndex < e_NUMBER_OF_ACCESS_LOG_TABLE_COLUMNS;iIndex++ )
                {
                    switch( iIndex )
                    {
                    case e_ACCESS_LOG_TABLE_DATE_TIME_COL:
                        sTempString = QString(", %1").arg( FormatDateTimeString( query.value(iIndex), dateFormat, timeFormat ) );
                        break;
                    default:
                        sTempString = QString(", %1").arg(query.value(iIndex).toString());
                        break;
                    }

                    sWriteBuffer = sWriteBuffer.append( sTempString );
                }

                sWriteBuffer = sWriteBuffer.append("\r\n");

                qDebug() << sWriteBuffer;
                CSVFile.write(sWriteBuffer.toAscii(), sWriteBuffer.length());
            }
        }
    }
    else
    {
        QString sErrorMessage = "iC3_DatabaseAccessLogTable::getLastEventSequenceIndex() - Database is not open";
        SetLastError( sErrorMessage );
        qDebug() << m_sLastError;
        return false;
    }

    CSVFile.flush();
    CSVFile.close();

    return bRC;
}
