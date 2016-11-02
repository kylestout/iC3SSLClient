#include <QSqlError>
#include <QDebug>
#include <QVariant>

#include "iC3_TransducerTable.h"


iC3_TransducerTable::iC3_TransducerTable()
{
    iC3_DatabaseColumnDef * pColumn;

    m_sTableName = "Transducers";

    setNumberOfColumns( e_NUMBER_OF_TRANSDUCER_TABLE_COLUMNS );

//    pColumn = new iC3_DatabaseColumnDef( tr("eventSequenceIndex"), "BIGINT UNSIGNED", "PRIMARY KEY" );  //64 bit
//    AddColumnDef( e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL, pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("dateTime"), "DATETIME", "" );
    AddColumnDef( e_TRANSDUCER_TABLE_DATE_TIME_COL , pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("RTD1Temperature"), "FLOAT", "" );
    AddColumnDef( e_TRANSDUCER_TABLE_RTD_1_TEMP_COL, pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("RTD2Temperature"), "FLOAT", "" );
    AddColumnDef( e_TRANSDUCER_TABLE_RTD_2_TEMP_COL, pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("RTD3Temperature"), "FLOAT", "" );
    AddColumnDef( e_TRANSDUCER_TABLE_RTD_3_TEMP_COL, pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("RTD4Temperature"), "FLOAT", "" );
    AddColumnDef( e_TRANSDUCER_TABLE_RTD_4_TEMP_COL, pColumn );

    pColumn = new iC3_DatabaseColumnDef( tr("RTD5Temperature"), "FLOAT", "" );
    AddColumnDef( e_TRANSDUCER_TABLE_RTD_5_TEMP_COL, pColumn );
}


QString iC3_TransducerTable::getTableCreationSQL( void )
{
    return iC3_DatabaseTable::getTableCreationSQL( m_sTableName );
}

bool iC3_TransducerTable::CreateTable( QSqlDatabase & database )
{
    bool bRC = false;
    QString sCreationSQL;
    QSqlQuery query( database );

    ClearLastError();

    if ( database.isOpen() )
    {
        sCreationSQL = getTableCreationSQL();
//        qDebug() << sCreationSQL;
        bRC = query.exec( sCreationSQL );

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_TransducerTable::CreateTable() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }
    }
    else
    {
        QString sErrorMessage = "iC3_TransducerTable::CreateTable() - Database is not open";
        SetLastError( sErrorMessage );
        qDebug() << m_sLastError;
        return false;
    }

    return bRC;
}

QString iC3_TransducerTable::getSQL_ColumnNames( void )
{
    QString sColumnNames;

    int iIndex;

    sColumnNames = QString("( %1").arg(getColumnDef(0)->getColumnName());

    for (iIndex = 1; iIndex < e_NUMBER_OF_TRANSDUCER_TABLE_COLUMNS; iIndex++ )
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
QString iC3_TransducerTable::getSQL_ColumnValues(  iC3_AccessLogData *pAccessLogEntry )
{
    QString sValueString;

//    QDateTime dtAccessDateTime = QDateTime( pAccessLogEntry->getAccessDate(),
//                                            pAccessLogEntry->getAccessTime(),
//                                            Qt::LocalTime );

//    sValueString = QString("VALUES ( ");
//    sValueString.append(QString("%1, ").arg( pAccessLogEntry->getAccessEvent() ));
//    sValueString.append(QString("%1, ").arg( SQL_FormatString(pAccessLogEntry->getUser())));
//    sValueString.append(QString("%1, ").arg( SQL_FormatQDateTime(dtAccessDateTime)));
//    sValueString.append(QString("%1, ").arg( pAccessLogEntry->getAccessMethodEnum()));
//    sValueString.append(QString("%1, ").arg( pAccessLogEntry->getIntDurationMinutes()));
//    sValueString.append(QString("%1, ").arg( SQL_FormatBoolean(pAccessLogEntry->getAccessEventViewed())));
//    sValueString.append(QString("%1, ").arg( SQL_FormatBoolean(pAccessLogEntry->getAccessEventDownloaded())));
//    sValueString.append(QString("%1 )").arg( pAccessLogEntry->getEventSequenceIndex()));

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
bool iC3_TransducerTable::updateAccessLogDataFromQuery( iC3_AccessLogData * pAccessLogEntry, QSqlQuery & query )
{
    QDateTime accessDateTime;

    if (    ( query.isValid() )
         && ( pAccessLogEntry != NULL )
       )
    {
//        pAccessLogEntry->setAccessEvent(query.value(e_ACCESS_LOG_TABLE_ACCESSS_EVENT_COL).toInt());
//        pAccessLogEntry->setUser( query.value(e_ACCESS_LOG_TABLE_USER_NAME_COL).toString());
//        accessDateTime = query.value(e_ACCESS_LOG_TABLE_DATE_TIME_COL).toDateTime();
//        pAccessLogEntry->setAccessDate(accessDateTime.date());
//        pAccessLogEntry->setAccessTime(accessDateTime.time());
//        pAccessLogEntry->setAccessMethod(query.value( e_ACCESS_LOG_TABLE_ACCESS_METHOD_COL).toInt());
//        pAccessLogEntry->setDurationMinutes(query.value(e_ACCESS_LOG_TABLE_ACCESS_DURATION_COL).toInt());
//        pAccessLogEntry->setAccessEventViewed(query.value( e_ACCESS_LOG_TABLE_VIEWED_COL).toBool());
//        pAccessLogEntry->setAccessEventDownloaded(query.value(e_ACCESS_LOG_TABLE_DOWNLOADED_COL).toBool());
//        pAccessLogEntry->setEventSequenceIndex(query.value(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL).toLongLong());
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
bool iC3_TransducerTable::insertNewEntry( QSqlDatabase &database,
                                          double fRTD1Val,
                                          double fRTD2Val,
                                          double fRTD3Val,
                                          double fRTD4Val,
                                          double fRTD5Val )
{
    QString queryString;
    QSqlQuery query( database );
    bool bRC = false;

    ClearLastError();

    if ( database.isOpen() )
    {
        queryString = QString( "INSERT INTO %1 ").arg(m_sTableName);

        queryString.append( getSQL_ColumnNames());

        queryString.append(" VALUES ( ");
        queryString.append(QString("%1, ").arg( SQL_FormatQDateTime(QDateTime::currentDateTime())));
        queryString.append(QString("%1, ").arg( SQL_FormatDouble(fRTD1Val)));
        queryString.append(QString("%1, ").arg( SQL_FormatDouble(fRTD2Val)));
        queryString.append(QString("%1, ").arg( SQL_FormatDouble(fRTD3Val)));
        queryString.append(QString("%1, ").arg( SQL_FormatDouble(fRTD4Val)));
        queryString.append(QString("%1 )").arg( SQL_FormatDouble(fRTD5Val)));

//        qDebug() << queryString;

        bRC = query.exec(queryString);

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_TransducerTable::InsertNewEntry() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            return false;
        }
    }
    else
    {
        QString sErrorMessage = "iC3_TransducerTable::InsertNewEntry() - Database is not open";
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
bool iC3_TransducerTable::getAccessLogEntry( QSqlDatabase & database,
                                                    quint32 ulEventSequenceIndex,
                                                    iC3_AccessLogData * pAccessLogEntry )
{
    bool bRC = false;
    QSqlQuery query( database );
    QString queryString;

    if ( database.isOpen() )
    {
        queryString = QString( "SELECT * FROM %1 WHERE %2 = %3").arg(m_sTableName).arg(getColumnDef(e_NUMBER_OF_TRANSDUCER_TABLE_COLUMNS)->getColumnName()).arg(ulEventSequenceIndex);
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
bool iC3_TransducerTable::getLastMultAccessLogData( QSqlDatabase &database,
                                                           unsigned int uiNumberOfEntries,
                                                           iC3_DatabaseAccessLogData * pReturnData )
{
    bool bRC = false;

    iC3_AccessLogData * pAccessLogEntry;
    QString sColumnName;
    QString queryString;
    QSqlQuery query( database );

    ClearLastError();

    sColumnName = m_qlColumnDefinitions.at(e_NUMBER_OF_TRANSDUCER_TABLE_COLUMNS)->getColumnName();

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
//            pAccessLogEntry = new iC3_AccessLogData();

            if( pAccessLogEntry == NULL )
            {
                SetLastError(QString("iC3_DatabaseAccessLogTable::getLastMultAccessLogData()-unable to create a new iC3_AccessLogData object"));
                return false;
            }

            if ( updateAccessLogDataFromQuery( pAccessLogEntry, query ) == true )
            {
//                pReturnData->AddAccessLogEntry(pAccessLogEntry);
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
bool iC3_TransducerTable::updateAccessLogEntry( QSqlDatabase & database,
                                                       iC3_AccessLogData *pAccessLogEntry )
{
    bool bRC = false;
    QSqlQuery query( database );
    QString queryString;

//    if ( database.isOpen() )
//    {
//        quint32 ulEventSequenceIndex =  pAccessLogEntry->getEventSequenceIndex();

//        QDateTime dtAccessDateTime = QDateTime( pAccessLogEntry->getAccessDate(),
//                                                pAccessLogEntry->getAccessTime(),
//                                                Qt::LocalTime );

//        queryString = QString( "UPDATE %1 SET ").arg(m_sTableName);

//        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_ACCESSS_EVENT_COL)->getColumnName()).arg(pAccessLogEntry->getAccessEvent()));
//        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_USER_NAME_COL)->getColumnName()).arg(SQL_FormatString(pAccessLogEntry->getUser())));
//        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_DATE_TIME_COL)->getColumnName()).arg(SQL_FormatQDateTime(dtAccessDateTime)));
//        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_ACCESS_METHOD_COL)->getColumnName()).arg(pAccessLogEntry->getAccessMethodEnum()));
//        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_ACCESS_DURATION_COL)->getColumnName()).arg(pAccessLogEntry->getIntDurationMinutes()));
//        queryString.append( QString("%1 = %2, ").arg(getColumnDef(e_ACCESS_LOG_TABLE_VIEWED_COL)->getColumnName()).arg(SQL_FormatBoolean(pAccessLogEntry->getAccessEventViewed())));
//        queryString.append( QString("%1 = %2 ").arg(getColumnDef(e_ACCESS_LOG_TABLE_DOWNLOADED_COL)->getColumnName()).arg(SQL_FormatBoolean(pAccessLogEntry->getAccessEventDownloaded())));

//        queryString.append( QString("WHERE %1 = %2").arg(m_qlColumnDefinitions.at(e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL)->getColumnName()).arg(ulEventSequenceIndex));

//        qDebug() << queryString;
//        bRC = query.exec(queryString);

//        if ( bRC == false )
//        {
//            QString sQueryError = query.lastError().text();
//            SetLastError( QString("iC3_DatabaseAccessLogTable::updateAccessLogEntry() - Query Error: %1").arg(sQueryError));
//            qDebug() << m_sLastError;
//            return false;
//        }
//    }
//    else
//    {
//         QString sErrorMessage = "iC3_DatabaseAccessLogTable::updateAccessLogEntry() - Database is not open";
//         SetLastError( sErrorMessage );
//         qDebug() << m_sLastError;
//         return false;
//    }

    return bRC;
}
