#ifndef IC3_DATABASE_H
#define IC3_DATABASE_H

/**
*     @file iC3_Database.h
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief this header file defines the iC3_Database class.  This class is used to
*           create valid database requests and add the requests to the database request
*           queue.  Database requests in the queue will be processed by the
*           iC3_DatabaseRequestProcessor which runs in a separate thread.
*/

#include <QSqlDatabase>
#include <QObject>

#include "iC3_DMM_Constants.h"
#include "iC3_TransducerTable.h"


class iC3_Database : public QObject
{
    Q_OBJECT
public:
    explicit iC3_Database(QObject *parent = 0);
    ~iC3_Database( );

//    bool performDB_IntegrityCheck( void );
//    bool LogEventData( uint uiTransactionID, iC3_EventLogData * pEventData );
//    bool getEventData( uint uiTransactionID, quint32 ulEventSequenceIndex );
//    bool getLastMultiEvents(  uint uiTransactionID, unsigned int uiNumberOfEvents );
//    bool updateEventsViewed( quint32 ulBeginESIN, quint32 ulEndESIN );
//    bool getEventsInRange( uint uiTransactionID, quint32 ulBeginEventSequenceIndex, quint32 ulEndEventSequenceIndex);
//    bool getGraphAlarmData( iC3_GraphAlarmFrame * pAlarmFrame, eEventLogTypes eEventLogType );
//    bool modifyEventLogEntry( uint uiTransactionID, iC3_EventLogData * pEventData );
//    bool getDoorOpeningsToday( uint uiTransactionID );
//    bool getDoorOpeningsForDateRange( uint uiTransactionID, QDate beginDateRange, QDate endDateRange );
//    bool getTotalDoorOpenings( uint uiTransactionID );
//    bool InsertTransSyncEntry( uint uiTransactionID, iC3_DatabaseTransSyncEntry * pEntry );
//    bool getTransducerSyncData( uint uiTransactionID, quint32 ulBeginSequenceIndex, quint32 ulEndSequenceIndex );
//    bool insertNewAccessControlEntry( uint uiTransactionID, iC3_AccessControlData * pData);
//    bool deleteAccessControlEntry( uint uiTransactionID, QString sUserName );
//    bool getAccessControlEntry( uint uiTransactionID, QString sUserName );
//    bool updateAccessControlEntry(uint uiTransactionID, iC3_AccessControlData * pData);
//    bool getAllAccessControlData(uint uiTransactionID);
//    bool insertNewAccessLogEntry( uint uiTransactionID, iC3_AccessLogData *pData );
//    bool getAccessLogEntry( uint uiTransactionID, quint32 ulEventSequenceIndex );
//    bool updateAccessLogEntry( uint uiTransactionID, iC3_AccessLogData * pData  );
//    bool updateAccessLogViewed( quint32 ulBeginESIN,  quint32 ulEndESIN );
//    bool getLastMultAccessLogData(uint uiTransactionID, uint uiNumberOfEntries );
//    bool insertAuthorizedUser( uint uiTransactionID, iC3_DatabaseAuthorizedUserEntry * pAuthorizedUser );
//    bool getAuthorizedUser( uint uiTransactionID, QString sUserID );
//    bool modifyAuthorizedUser( uint uiTransactionID, iC3_DatabaseAuthorizedUserEntry * pAuthorizedUser );
//    bool insertUserEvent( uint uiTransactionID, iC3_DatabaseUserEventsEntry * pUserEvent );
//    bool getUserEvent( uint uiTransactionID, uint uiEventID );
//    bool modifyUserEvent( uint uiTransactionID, iC3_DatabaseUserEventsEntry * pUserEvent );
//    bool insertDefrostEvent( uint uiTransactionID, iC3_DatabaseDefrostEntry * pDefrostEvent );
//    bool getDefrostEvent( uint uiTransactionID, uint uiDefrostEventID );
//    bool modifyDefrostEvent( uint uiTransactionID, iC3_DatabaseDefrostEntry * pDefrostEvent );
//    bool getGraphDoorOpeningData( uint &uiTransactionID, quint32 ulGraphBeginSequenceIndex, quint32 ulGraphEndSequenceIndex, QVector< iC3_GraphPixelFrame *> * pGraphPixelFrames );
//    bool getGraphEpochData( uint &uiTransactionID,  iC3_DMM_GraphEpochData * pEpochData, uint uiNumberOfDaysToGraph, uint uiTransducerID );

//    bool getESIN_RangeForCSV_Generation( uint uiTransactionID, quint16 uiTransducerID, quint32 ulLastESIN, eDownloadDataSelection eSelection );
//    bool write_CSV_Files( uint uiTransactionID, QString sUnitIDNum, QDateTime now, quint32 ulBeginSequenceIndex, quint32 ulEndSequenceIndex, eDateFormats dateFormat, eTimeFormats timeFormat );
//    void abortCSV_FileGeneration( void );

//    void make_CSV_Connections( iC3_DMM_CSV_FileGenerator * pCSV_FileGenerator );
//    void break_CSV_Connections( iC3_DMM_CSV_FileGenerator * pCSV_FileGenerator );

    bool openDatabase( void );
    void closeDatabase( void );
//    bool commErrorMoveDatabase( void );

//    void setInitialAlarmLimits( uint uiTransducerID, float fLowerAlarmLimit, float fUpperAlarmLimit );
//    void setInterfacePointer( iC3_DMM_Interface * pInterface );

signals:

//    void signalError(eDMM_ErrorLevels errorLevel, QString sErrorMessage );
//    void signalCommError( eDMM_CommErrorLevels eCommErrorLevel, QString sErrorMessage );

//    void signalGraphEpochData( uint uiTransactionID );
//    void signalGraphDoorOpenData( uint uiTransactionID );

public slots:
    bool insertTransducerEntry( double fRTD1Val,
                                double fRTD2Val,
                                double fRTD3Val,
                                double fRTD4Val,
                                double fRTD5Val );

//    void handleCommError( eDMM_CommErrorLevels eCommErrorLevel );
//    void handleGraphEpochData( uint uiTransactionID );
//    void handleGraphDoorOpenData( uint uiTransactionID );

private:

    QString m_sDatabaseFileName;

    uint getTransactionID( void );

//    iC3_DatabaseRequestProcessor m_RequestProcessor;
    QSqlDatabase m_db;
    bool m_bDatabaseOpen;

//    iC3_DMM_Interface * m_pInterfacePtr;

//    QMutex m_DB_RequestMutex;
//    QMutex m_TransactionIDMutex;
    uint m_uiTransactionID;

    iC3_TransducerTable m_TransducerTable;

};

#endif // IC3_DATABASE_H
