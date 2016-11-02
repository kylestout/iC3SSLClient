#ifndef iC3_DATABASEACCESSLOGTABLE_H
#define iC3_DATABASEACCESSLOGTABLE_H
/**
*     @file iC3_DatabaseAccessLogTable.h
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief This header file defines the iC3_DatabaseAccessLogTable class.  This class is
*            responsible for creating, reading from and writing to the Access Log Table in
*            the database.
*/

#include <QFile>
#include "iC3_DatabaseTable.h"

class iC3_DatabaseAccessLogTable : public iC3_DatabaseTable
{
public:
    iC3_DatabaseAccessLogTable();

    bool insertNewEntry( QSqlDatabase & database, iC3_AccessLogData * pEntry );

    bool getAccessLogEntry( QSqlDatabase & database,
                            quint32 ulEventSequenceIndex,
                            iC3_AccessLogData * pAccessLogEntry );

    bool updateAccessLogEntry( QSqlDatabase & database,
                               iC3_AccessLogData * pAccessLogEntry );

    bool getLastMultAccessLogData( QSqlDatabase & database, unsigned int uiNumberOfEvents, iC3_DatabaseAccessLogData *pAccessLogData );

    bool write_CSV_File( QSqlDatabase &database,
                         iC3_DatabaseRequest * pRequest,
                         eDateFormats dateFormat,
                         eTimeFormats timeFormat );

    bool setEventsDownloaded( QSqlDatabase & database,
                              quint32 ulBeginSequenceIndex,
                              quint32 ulEndSequenceIndex );

    bool updateViewedEvents( QSqlDatabase & database,
                             quint32 ulBeginESIN,
                             quint32 ulEndESIN );



    enum eIC3_AccessLogTableColumns
    {
        e_ACCESS_LOG_TABLE_ACCESSS_EVENT_COL        = 0,
        e_ACCESS_LOG_TABLE_USER_NAME_COL            = 1,
        e_ACCESS_LOG_TABLE_DATE_TIME_COL            = 2,
        e_ACCESS_LOG_TABLE_ACCESS_METHOD_COL        = 3,
        e_ACCESS_LOG_TABLE_ACCESS_DURATION_COL      = 4,
        e_ACCESS_LOG_TABLE_VIEWED_COL               = 5,
        e_ACCESS_LOG_TABLE_DOWNLOADED_COL           = 6,
        e_ACCESS_LOG_TABLE_EVENT_SEQUENCE_INDEX_COL = 7,

        e_NUMBER_OF_ACCESS_LOG_TABLE_COLUMNS
    };

    QString getTableCreationSQL( void );

    bool CreateTable( QSqlDatabase & database );

private:

    QString getSQL_ColumnNames( void );
    QString getSQL_ColumnValues( iC3_AccessLogData *pAccessLogEntry );
    bool    updateAccessLogDataFromQuery( iC3_AccessLogData * pAccessLogEntry, QSqlQuery & query );
};

#endif // iC3_DATABASEACCESSLOGTABLE_H
