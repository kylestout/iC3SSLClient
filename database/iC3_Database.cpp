/**
*     @file iC3_Database.cpp
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief this cpp file implements the iC3_Database class.  This class is used to
*           create valid database requests and add the requests to the database request
*           queue.  Database requests in the queue will be processed by the
*           iC3_DatabaseRequestProcessor which runs in a separate thread.
*/

#include <QDebug>
#include <QSqlError>
#include <limits>
#include "iC3_Database.h"

//-----------------------------------------------------------------------------------------------
/** constructor
*   @param parent - QObject pointer parent (unused)
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_Database::iC3_Database(QObject *parent) :
    QObject(parent),
    m_bDatabaseOpen(false),
//    m_pInterfacePtr( NULL ),
    m_uiTransactionID(0)
{
    m_sDatabaseFileName = QString(HELMER_DATABASE_FILE_NAME);
}

//-----------------------------------------------------------------------------------------------
/** destructor
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_Database::~iC3_Database( )
{
    if ( m_bDatabaseOpen )
    {
        closeDatabase();
    }
}

//-----------------------------------------------------------------------------------------------
/** openDatabase() - Used to open the Helmer Database
*   @retval true - the database was opened
*   @retval false - the database open failed - signalError is emitted
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_Database::openDatabase( )
{
//    iC3_DatabaseRequest * pRequest;

    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

    m_db = QSqlDatabase::addDatabase("QSQLITE", HELMER_DB_CONNECTION_NAME );
    m_db.setHostName("localhost");
    m_db.setDatabaseName( m_sDatabaseFileName );
    m_db.setUserName("root");
    m_db.setPassword("");

    if ( !m_db.open() )
    {
        QString sDBError = m_db.lastError().text();
        QString sErrorMsg = QString("Unable to open the database: %1").arg( sDBError );
        qDebug() << sErrorMsg;
        return false;
    }


    /////////////////////////////////////////////////////////////////////////////// TRANSDUCER TABLE
    bool bRC;
    bRC = m_TransducerTable.CreateTable( m_db );
    if ( bRC == false )
    {
        qDebug() << m_TransducerTable.GetLastError();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------------------------
/** closeDatabase() - Used to close the Helmer Database
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_Database::closeDatabase( void )
{
//    m_RequestProcessor.stopProcessingDbRequests();
    m_db.close();

    m_bDatabaseOpen = false;

//    // these connections are used to communicate database request status to the graph generation classes
//    disconnect( &m_RequestProcessor, SIGNAL(signalGraphEpochData(uint)), this, SLOT(handleGraphEpochData(uint)));
//    disconnect( &m_RequestProcessor, SIGNAL(signalGraphDoorOpenData(uint)), this, SLOT(handleGraphDoorOpenData(uint)));
}

bool iC3_Database::insertTransducerEntry( double fRTD1Val,
                                          double fRTD2Val,
                                          double fRTD3Val,
                                          double fRTD4Val,
                                          double fRTD5Val )
{
    return m_TransducerTable.insertNewEntry( m_db, fRTD1Val, fRTD2Val, fRTD3Val, fRTD4Val, fRTD5Val);
}


////-----------------------------------------------------------------------------------------------
///** getTransactionID() - get a unique transaction ID and pass it back to the calling function.
//*                        This is used to identify the request when the a transducer log request
//*                        returns a response.
//*   @retval - transaction ID (uint) value between 0 and 32767
//*   @author Doug Sanqunetti
//*   @date 12/13/2013
//*/
////-----------------------------------------------------------------------------------------------
//uint iC3_Database::getTransactionID( void )
//{
//    uint returnID;
//    m_TransactionIDMutex.lock();

//    m_uiTransactionID++;
//    if( m_uiTransactionID > 32767 )
//    {
//        m_uiTransactionID = 0;
//    }

//    returnID = m_uiTransactionID;

//    m_TransactionIDMutex.unlock();

//    return returnID;
//}

////-----------------------------------------------------------------------------------------------
///** setInterfacePointer() - Setting this pointer allows the connection of a SIGNAL emitted from
//*         this class to be connected to the interface pointer class object SLOT.
//*   @param  pInterface - pointer to an instance of a class with a SLOT designed to receive
//*                        SIGNALS from this class
//*   @retval void
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::setInterfacePointer( iC3_DMM_Interface * pInterface )
//{
//    m_pInterfacePtr = pInterface;

//    qRegisterMetaType< eDMM_ErrorLevels >("eDMM_ErrorLevels");
//    qRegisterMetaType< eEventLogTypes >("eEventLogTypes");

//    connect( &m_RequestProcessor, SIGNAL(signalCommError(eDMM_CommErrorLevels,QString)), m_pInterfacePtr, SLOT(handleCommError(eDMM_CommErrorLevels,QString)));
//    connect( &m_RequestProcessor, SIGNAL(signalSuccess(uint)), m_pInterfacePtr, SLOT(handleDatabaseSuccess(uint)));
//    connect( &m_RequestProcessor, SIGNAL(signalGraphAlarmData(iC3_DatabaseEventData*,eEventLogTypes)), m_pInterfacePtr, SLOT(handleGraphAlarmData( iC3_DatabaseEventData*, eEventLogTypes)));
//    connect( &m_RequestProcessor, SIGNAL(signalEventData(uint,iC3_DatabaseEventData*)), m_pInterfacePtr, SLOT(handleDatabaseEventData(uint,iC3_DatabaseEventData*)));
//    connect( &m_RequestProcessor, SIGNAL(signalDoorOpeningsData(uint,uint)), m_pInterfacePtr, SLOT(handleDoorOpeningData(uint,uint)));
//    connect( &m_RequestProcessor, SIGNAL(signalTransducerSyncData(uint,iC3_DatabaseTransSyncData*)), m_pInterfacePtr, SLOT(handleTransSyncData(uint,iC3_DatabaseTransSyncData*)));
//    connect( &m_RequestProcessor, SIGNAL(signalAccessControlEntry(uint,iC3_AccessControlData*)), m_pInterfacePtr, SLOT(handleAccessControlEntry(uint,iC3_AccessControlData*)));
//    connect( &m_RequestProcessor, SIGNAL(signalAllAccessControlData( uint, iC3_DatabaseAccessControlData *)), m_pInterfacePtr, SLOT(handleAllAccessControlData(uint,iC3_DatabaseAccessControlData*)));
//    connect( &m_RequestProcessor, SIGNAL(signalAccessLogData(uint,iC3_DatabaseAccessLogData*)), m_pInterfacePtr, SLOT(handleAccessLogData(uint,iC3_DatabaseAccessLogData*)));
//    connect( &m_RequestProcessor, SIGNAL(signalAccessLogEntry(uint,iC3_AccessLogData*)), m_pInterfacePtr, SLOT(handleAccessLogEntry(uint,iC3_AccessLogData*)));
//    connect( &m_RequestProcessor, SIGNAL(signalAuthorizedUser(uint,iC3_DatabaseAuthorizedUserEntry*)), m_pInterfacePtr, SLOT(handleAuthorizedUserEntry(uint,iC3_DatabaseAuthorizedUserEntry*)));
//    connect( &m_RequestProcessor, SIGNAL(signalUserEvent(uint,iC3_DatabaseUserEventsEntry*)), m_pInterfacePtr, SLOT(handleUserEventsEntry(uint,iC3_DatabaseUserEventsEntry*)));
//    connect( &m_RequestProcessor, SIGNAL(signalDefrostEvent( uint, iC3_DatabaseDefrostEntry*)), m_pInterfacePtr, SLOT(handleDefrostEvent(uint,iC3_DatabaseDefrostEntry*)));
//    connect( &m_RequestProcessor, SIGNAL(signalDB_IntegrityCheckComplete()), m_pInterfacePtr, SLOT(handleDB_IntegrityCheckComplete()));
//}

////-----------------------------------------------------------------------------------------------
///** make_CSV_Connections() - makes signal/slot connections used only during CSV file generation
//*   @param  pCSV_FileGenerator - pointer to an instance of the iC3-CSV_FileGenerator class
//*                                where SLOTs exist to receive CSV File Generation SIGNALS
//*   @retval void
//*   @author Doug Sanqunetti
//*   @date 5/22/2014
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::make_CSV_Connections( iC3_DMM_CSV_FileGenerator *pCSV_FileGenerator )
//{
//    connect( &m_RequestProcessor, SIGNAL(signalDB_CSV_FilesComplete(uint, quint32, quint32)), pCSV_FileGenerator, SLOT(handleDB_CSV_FilesComplete(uint, quint32, quint32)));
//    connect( &m_RequestProcessor, SIGNAL(signalCSV_FileGenerationError(QString)), pCSV_FileGenerator, SLOT(handleDBCSV_FileGenerationError(QString)));
//    connect( &m_RequestProcessor, SIGNAL(signalCSV_FileGenerationAborted()), pCSV_FileGenerator, SLOT(handleCSV_FileGenerationAborted()));
//    connect( &m_RequestProcessor, SIGNAL(signalESIN_RangeForCSV_Generation(uint,quint32,quint32)), pCSV_FileGenerator, SLOT(handleESIN_RangeForCSV_Generation(uint,quint32,quint32)));
//}

////-----------------------------------------------------------------------------------------------
///** break_CSV_Connections() - breaks signal/slot connections used only during CSV file generation
//*   @param  pCSV_FileGenerator - pointer to an instance of the iC3-CSV_FileGenerator class
//*                                where SLOTs exist to receive CSV File Generation SIGNALS
//*   @retval void
//*   @author Doug Sanqunetti
//*   @date 5/22/2014
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::break_CSV_Connections( iC3_DMM_CSV_FileGenerator * pCSV_FileGenerator )
//{
//    disconnect( &m_RequestProcessor, SIGNAL(signalDB_CSV_FilesComplete(uint, quint32, quint32)), pCSV_FileGenerator, SLOT(handleDB_CSV_FilesComplete(uint, quint32, quint32)));
//    disconnect( &m_RequestProcessor, SIGNAL(signalCSV_FileGenerationError(QString)), pCSV_FileGenerator, SLOT(handleCSV_FileGenerationError(QString)));
//    disconnect( &m_RequestProcessor, SIGNAL(signalCSV_FileGenerationAborted()), pCSV_FileGenerator, SLOT(handleCSV_FileGenerationAborted()));
//    disconnect( &m_RequestProcessor, SIGNAL(signalESIN_RangeForCSV_Generation(uint,quint32,quint32)), pCSV_FileGenerator, SLOT(handleESIN_RangeForCSV_Generation(uint,quint32,quint32)));
//}

////-----------------------------------------------------------------------------------------------
///** performDB_IntegrityCheck() - Add a request to perform a database integrity check to the
//*                                database request processing queue
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 04/01/2014
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::performDB_IntegrityCheck( void )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_PERFORM_INTEGRITY_CHECK );

//    if ( pRequest != NULL )
//    {
//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** LogEventData() - creates a database request to log an Event
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded
//*   @param  pEventData - pointer to an iC3_EventLogData object containing the event data to be
//*                        logged.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::LogEventData( unsigned int uiTransactionID, iC3_EventLogData *pEventData )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    if ( pEventData == NULL )
//    {
//        return false;
//    }

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_ADD_NEW_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setEventLogData( pEventData );
//        pRequest->setTransactionID( uiTransactionID );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getEventData() - creates a database request to retrieve a previously logged event
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded
//*   @param  ulEventSequenceIndex - the Event Sequence Index of the event to be retrieved
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getEventData( uint uiTransactionID, quint32 ulEventSequenceIndex )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex( ulEventSequenceIndex );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getLastMultiEvents() - creates a database request to retrieve the last n previously logged
//*                          events
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded
//*   @param  uiNumberOfEvents - specifies how many events to retrieve.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getLastMultiEvents( uint uiTransactionID, unsigned int uiNumberOfEvents )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_LAST_N_EVENTS );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setNumberOfEvents( uiNumberOfEvents );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** updateEventsViewed() - creates a database request to update the "viewed" status of events
//*                          listed in the ulESIN_List
//*   @param  ulBeginESIN - the first ESIN in the range of events to be updated
//*   @param  ulEndESIN - the last ESIN in the range of events to be updated
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 5/30/2014
//*/
////-----------------------------------------------------------------------------------------------
//bool  iC3_Database::updateEventsViewed( quint32 ulBeginESIN,  quint32 ulEndESIN )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;
//    uint uiTransactionID;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_UPDATE_VIEWED_EVENTS );

//    if ( pRequest != NULL )
//    {
//        uiTransactionID = getTransactionID();
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex( ulBeginESIN );
//        pRequest->setEndSequenceIndex( ulEndESIN );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getEventsInRange() - initiaties a database request to retrieve all events witin the
//*                        EventSequenceIndexRange starting with the ulBeginEventSequenceIndex and
//*                        ending with the ulEndEventSequenceIndex inclusive.
//*   @param  uiTransactionID - a unique identifier for this transaction
//*   @param  ulBeginEventSequenceIndex - the beginning of the event sequence index range
//*   @param  ulEndEventSequenceIndex - the ending of the event sequence index range
//*   @retval true - the database request was queued
//*   @retval false - the database request could not be queued
//*   @author Doug Sanqunetti
//*   @date 11/29/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getEventsInRange( uint uiTransactionID,
//                                     quint32 ulBeginEventSequenceIndex,
//                                     quint32 ulEndEventSequenceIndex)
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_EVENTS_IN_ESIN_RANGE );
//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex( ulBeginEventSequenceIndex );
//        pRequest->setEndSequenceIndex( ulEndEventSequenceIndex );

//        bRC = m_RequestProcessor.AddRequestToQueue( pRequest );
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getGraphAlarmData() - initiaties a database request to retrieve all events related to a graph
//*                      alarm icon (iC3_GraphAlarmFrame)
//*   @param  pAlarmFrame - a pointer to an iC3_GraphAlarmFrame object containing event ESIN's to
//*                      retrieve
//*   @param  eEventLogType - enumerated event log type
//*   @retval true - the database request was queued
//*   @retval false - the database request could not be queued
//*   @author Doug Sanqunetti
//*   @date 01/08/2014
//*   @note   Because this is a request from the GUI and a user will be waiting for a response,
//*           the request is placed at the front of the queue.
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getGraphAlarmData( iC3_GraphAlarmFrame * pAlarmFrame, eEventLogTypes eEventLogType )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_GRAPH_EVENT_DATA );
//    if ( pRequest != NULL )
//    {
//        pRequest->setGraphAlarmFrame( pAlarmFrame );
//        pRequest->setEventLogType( eEventLogType );

//        bRC = m_RequestProcessor.AddRequestToFrontOfQueue( pRequest );
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** modifyEventLogEntry() - Modifies the details of a previously logged event entry.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded
//*   @param  pEventData - pointer to an iC3_EventLogData object containing the event data to be
//*                        logged.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::modifyEventLogEntry(uint uiTransactionID, iC3_EventLogData * pEventData )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_MODIFY_EXISTING_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setEventLogData( pEventData );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getDoorOpeningsToday() - Used to retrieve the number of door openings today.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getDoorOpeningsToday( unsigned int uiTransactionID )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_DOOR_OPENINGS_TODAY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID(uiTransactionID);
//        QDateTime nowTime = QDateTime::currentDateTime();
//        QDate Today = nowTime.date();
//        pRequest->setRequestDate( Today );

//        bRC = m_RequestProcessor.AddRequestToQueue( pRequest );
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getDoorOpeningsForDateRange() - Used to retrieve the number of door openings that occurred
//*                                   within a date ranged.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  beginDateRange - the beginning date in the requested date range
//*   @param  endDateRange - the ending date in the requested date range
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getDoorOpeningsForDateRange( unsigned int uiTransactionID,
//                                                QDate beginDateRange,
//                                                QDate endDateRange )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_DOOR_OPENINGS_DATE_RANGE );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID(uiTransactionID);
//        pRequest->setBeginDate(beginDateRange);
//        pRequest->setEndDate(endDateRange);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getTotalDoorOpenings() - Used to retrieve the total number of door openings
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getTotalDoorOpenings( unsigned int uiTransactionID )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_TOTAL_DOOR_OPENINGS);

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID(uiTransactionID);
//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** write_CSV_Files() - Used to generate CSV files from the database tables
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  sUnitIDNum - The units ID number used in the CSV file names to indicate which unit
//*                        is the source of the file after files have been downloaded.
//*   @param  now - The ending date in the date range of data to be included in the CSV files.
//*   @param  ulBeginSequenceIndex - the first ESIN in the range of data to be included in the CSV
//*                       file.
//*   @param  ulEndSequenceIndex - the last ESIN in the range of data to be included in the CSV
//*                       file.
//*   @param  dateFormat - enumerated date format to be used while writing dates in the CSV file
//*   @param  timeFormat - enumerated time format to be used while writing times in the CSV file
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::write_CSV_Files( uint uiTransactionID,
//                                    QString sUnitIDNum,
//                                    QDateTime now,
//                                    quint32 ulBeginSequenceIndex,
//                                    quint32 ulEndSequenceIndex,
//                                    eDateFormats dateFormat,
//                                    eTimeFormats timeFormat )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_WRITE_CSV_FILES);

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setUnitID( sUnitIDNum );
//        pRequest->setDateTime( now );
//        pRequest->setBeginSequenceIndex( ulBeginSequenceIndex );
//        pRequest->setEndSequenceIndex( ulEndSequenceIndex );
//        pRequest->setDateFormat( dateFormat );
//        pRequest->setTimeFormat( timeFormat );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }
//    else
//    {
//        QString sErrorMsg = QString("iC3_Database::write_CSV_Files() - Unable to create a new iC3_DatabaseRequest.");
//        emit signalError(eDMM_ERROR_LEVEL_WARNING, sErrorMsg );
//        bRC = false;
//    }
//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** abortCSV_FileGeneration() - Used to abort the generation of CSV files.  This can be called
//*         if the user removes the USB flash drive while the CSV files are being generated.
//*   @retval void
//*   @author Doug Sanqunetti
//*   @date 07/10/2014
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::abortCSV_FileGeneration( void )
//{
//    qDebug() << "iC3_Database::abortCSV_FileGeneration() was called";
//    m_RequestProcessor.abortCSV_FileGeneration();
//}

////-----------------------------------------------------------------------------------------------
///** InsertTransSyncEntry() - Used to add a transducer synchronization entry into the database.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pEntry - pointer to an iC3_DatabaseTransSyncEntry object containing the transducer
//*                    synchronization data to be added.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::InsertTransSyncEntry( uint uiTransactionID, iC3_DatabaseTransSyncEntry * pEntry )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_INSERT_TRANS_SYNC_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setTransSyncEntry( pEntry );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getTransducerSyncData() - Used to retrieve transducer synchronization entries within an
//*                             Event Sequence Index range from the database.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  ulBeginSequenceIndex - the beginning Event Sequence Index in the requested range.
//*   @param  ulEndSequenceIndex - the ending Event SequenceIndex in the requested range.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getTransducerSyncData( uint uiTransactionID, quint32 ulBeginSequenceIndex, quint32 ulEndSequenceIndex )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_TRANS_SYNC_DATA );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex( ulBeginSequenceIndex );
//        pRequest->setEndSequenceIndex( ulEndSequenceIndex );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** insertNewAccessControlEntry() - Used to insert a new Access Control entry into the database
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pData - pointer to an iC3_AccessControlData object containing the Access Control data
//*                   to be inserted.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::insertNewAccessControlEntry( uint uiTransactionID, iC3_AccessControlData * pData)
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_INSERT_ACCESS_CONTROL_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setAccessControlData( pData );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** deleteAccessControlEntry() - Used to delete a persons access to the unit by removing an
//*                                Access Control entry from the database
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  sUserName - user name of the person to be deleted from the Access Control Database.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::deleteAccessControlEntry( uint uiTransactionID, QString sUserName )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_DELETE_ACCESS_CONTROL_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setUserID( sUserName );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getAccessControlEntry() - Used to retrieve a specific Access Control entry from the database
//*                             using the User Name as the key
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  sUserName - user name of the person to be retrieved from the Access Control Database.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getAccessControlEntry( uint uiTransactionID, QString sUserName )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_ACCESS_CONTROL_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setUserID( sUserName );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** updateAccessControlEntry() - Used to update an existing Access Control entry in the database.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pData - pointer to an iC3_AccessControlData object containing the Access Control data
//*                   to be updated.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::updateAccessControlEntry(uint uiTransactionID, iC3_AccessControlData * pData)
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_MODIFY_ACCESS_CONTROL_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setAccessControlData( pData );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getAllAccessControlData() - Used to retrieve all existing Access Control entries in the
//*                               database.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getAllAccessControlData(uint uiTransactionID)
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_ALL_ACCESS_CONTROL_DATA );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** insertNewAccessLogEntry() - Used to insert a new Access Log Entry into the database.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pData - pointer to an iC3_AccessLogData object containing the data t be inserted
//*                   into the database.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::insertNewAccessLogEntry( uint uiTransactionID, iC3_AccessLogData * pData )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_INSERT_ACCESS_LOG_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setAccessLogData( pData );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getAccessLogEntry() - Used to retrieve an Access Log Entry from the database using the
//*                         Event Sequence Index as the key.
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  ulEventSequenceIndex - identifies which Access Log Entry to retrieve
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getAccessLogEntry( uint uiTransactionID, quint32 ulEventSequenceIndex )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_ACCESS_LOG_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex(ulEventSequenceIndex);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** updateAccessLogEntry() - Used to modify an existing Access Log Entry in the database
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pData - pointer to an iC3_AccessLogData object containing the Access Log Data to be
//*                   modified.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::updateAccessLogEntry( uint uiTransactionID, iC3_AccessLogData *pData  )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_UPDATE_ACCESS_LOG_ENTRY );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setAccessLogData(pData);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getLastMultAccessLogData() - Used to retrieve the last n Access Log Entries from the database
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  uiNumberOfEntries - the number of Access Log entries to retrieve.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 09/01/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getLastMultAccessLogData(uint uiTransactionID, uint uiNumberOfEntries )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_LAST_N_ACCESS_LOG_ENTRIES );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setNumberOfEvents(uiNumberOfEntries);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** updateAccessLogViewed() - creates a database request to update the "viewed" status of access
//*                          log entries listed in the ulESIN_List
//*   @param  ulBeginESIN - the first ESIN in the range of events to be updated
//*   @param  ulEndESIN - the last ESIN in the range of events to be updated
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 5/30/2014
//*/
////-----------------------------------------------------------------------------------------------
//bool  iC3_Database::updateAccessLogViewed( quint32 ulBeginESIN,  quint32 ulEndESIN )
//{
//    bool bRC = false;
//    iC3_DatabaseRequest * pRequest;
//    uint uiTransactionID;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_UPDATE_VIEWED_ACCESS_LOG );

//    if ( pRequest != NULL )
//    {
//        uiTransactionID = getTransactionID();
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex( ulBeginESIN );
//        pRequest->setEndSequenceIndex( ulEndESIN );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** insertAuthorizedUser() - Used to insert an authorized user entry into the authorized user table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pAuthorizedUser - pointer to an iC3_DatabaseAuthorizedUserEntry object containing the
//*                             authorized user data
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::insertAuthorizedUser( uint uiTransactionID, iC3_DatabaseAuthorizedUserEntry * pAuthorizedUser )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_INSERT_AUTHORIZED_USER );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setAuthorizedUserEntry( pAuthorizedUser );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getAuthorizedUser() - Used to retrieve an authorized user entry from the authorized user table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  sUserID - (QString) User-ID of the requested authorized user
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getAuthorizedUser( uint uiTransactionID, QString sUserID )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_AUTHORIZED_USER );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setUserID( sUserID );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** modifyAuthorizedUser() - Used to modify an authorized user entry in the authorized user table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pAuthorizedUser - pointer to an iC3_DatabaseAuthorizedUserEntry object containing the
//*                             authorized user data
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::modifyAuthorizedUser( uint uiTransactionID, iC3_DatabaseAuthorizedUserEntry * pAuthorizedUser )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_MODIFY_AUTHORIZED_USER );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setAuthorizedUserEntry( pAuthorizedUser);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** insertUserEvent() - Used to insert a user event entry into the User Events table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pUserEvent - pointer to an iC3_DatabaseUserEventsEntry object containing the
//*                        user event data
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::insertUserEvent( uint uiTransactionID, iC3_DatabaseUserEventsEntry * pUserEvent )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_INSERT_USER_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setUserEventsEntry(pUserEvent);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getUserEvent() - Used to retrieve a user event entry from the User Events table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  uiEventID - (uint) Event-ID of the requested user event
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getUserEvent( uint uiTransactionID, uint uiEventID )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_USER_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setEventID( uiEventID );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** modifyUserEvent() - Used to modify a user event entry in the User Events table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pUserEvent - pointer to an iC3_DatabaseUserEventsEntry object containing the
//*                            User Event data
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::modifyUserEvent( uint uiTransactionID,
//                                    iC3_DatabaseUserEventsEntry * pUserEvent )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_MODIFY_USER_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setUserEventsEntry( pUserEvent);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** insertDefrostEvent() - Used to insert a defrost event entry into the Defrost table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pDefrostEvent - pointer to an iC3_DatabaseDefrostEntry object containing the
//*                        defrost event data
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::insertDefrostEvent( uint uiTransactionID,
//                                       iC3_DatabaseDefrostEntry * pDefrostEvent )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_INSERT_DEFROST_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setDefrostEntry( pDefrostEvent);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getDefrostEvent() - Used to retrieve a defrost entry from the Defrost table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  uiDefrostEventID - (uint) DefrostEvent-ID of the requested defrost event
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getDefrostEvent( uint uiTransactionID, uint uiDefrostEventID )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_DEFROST_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setEventID( uiDefrostEventID );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** modifyDefrostEvent() - Used to modify a Defrost entry in the Defrost table
//*   @param  uiTransactionID - a unique identifier that is used when signaling success or failure
//*                             to identify which operation failed or succeeded.
//*   @param  pDefrostEvent - pointer to an iC3_DatabaseDefrostEntry object containing the
//*                             Defrost data
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 11/22/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::modifyDefrostEvent( uint uiTransactionID,
//                                       iC3_DatabaseDefrostEntry * pDefrostEvent )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_MODIFY_DEFROST_EVENT );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setDefrostEntry( pDefrostEvent);

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getGraphEpochData() - Used to retrieve graph epoch data from the transducer synchronization
//*                         table
//*   @param  uiTransactionID - a unique identifier that is assigned by this function and used when
//*                             signaling success or failure to the calling function
//*                             to identify which operation failed or succeeded.
//*   @param  pEpochData - pointer to a iC3_DMM_GraphEpochData object.  This is where the data will be
//*                        loaded
//*   @param  uiNumberOfDaysToGraph - this is the number of days that will be graphed so epoch data
//*                             for this number of days will be returned
//*   @param  ulLastESIN - the last ESIN that will be included in the graph.
//*   @param  uiTransducerID - the unique Transducer Identifier.  This is the transducer-ID used
//*                        when defining a transducer to be graphed
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 12/11/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getGraphEpochData( uint & uiTransactionID,
//                                      iC3_DMM_GraphEpochData * pEpochData,
//                                      uint uiNumberOfDaysToGraph,
//                                      uint uiTransducerID )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_GRAPH_EPOCH_DATA );

//    if ( pRequest != NULL )
//    {
//        uiTransactionID = getTransactionID();

//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setEpochData( pEpochData );
//        pRequest->setNumberOfDays( uiNumberOfDaysToGraph );
//        pRequest->setTransducerID( uiTransducerID );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getGraphDoorOpeningData() - Used to retrieve graph door opening data
//*   @param  uiTransactionID - a unique identifier that is assigned by this function and used when
//*                             signaling success or failure to the calling function
//*                             to identify which operation failed or succeeded.
//*   @param  ulGraphBeginSequenceIndex -  the first Event Sequence Index on the graph
//*   @param  ulGraphEndSequenceIndex - the last Event Sequence Index on the graph
//*   @param  pGraphPixelFrames - pointer to the array of iC3_GraphPixelFrame objects where the
//*                                 door opening data will be stored.
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 12/15/2013
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getGraphDoorOpeningData( uint &uiTransactionID,
//                                            quint32 ulGraphBeginSequenceIndex,
//                                            quint32 ulGraphEndSequenceIndex,
//                                            QVector< iC3_GraphPixelFrame *> * pGraphPixelFrames )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_GRAPH_DOOR_OPEN_DATA );

//    if ( pRequest != NULL )
//    {
//        uiTransactionID = getTransactionID();

//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setBeginSequenceIndex( ulGraphBeginSequenceIndex );
//        pRequest->setEndSequenceIndex( ulGraphEndSequenceIndex );
//        pRequest->setGraphPixelFrameArray( pGraphPixelFrames );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** getESIN_RangeForCSV_Generation() - Used to retrieve beginning and ending ESIN for CSV file
//*            generation based upon the eSelection argument (1-month or 2-months)
//*   @param  uiTransactionID - a unique identifier that is assigned by this function and used when
//*                             signaling success or failure to the calling function
//*                             to identify which operation failed or succeeded.
//*   @param  uiTransducerID -  used to calculate the ESINs based upon this transducer file and
//*                             events in the transducer sync table related to this transducer
//*   @param  eSelection - enumerated value (1-month or 2-months)
//*   @retval true - the request was added to the database request queue
//*   @retval false - the request was not successfully added to the database request queue
//*   @author Doug Sanqunetti
//*   @date 3/26/2014
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::getESIN_RangeForCSV_Generation( uint uiTransactionID,
//                                                   quint16 uiTransducerID,
//                                                   quint32 ulLastESIN,
//                                                   eDownloadDataSelection eSelection )
//{
//    bool bRC = false;

//    iC3_DatabaseRequest * pRequest;

//    // make sure our request is protected until we are able to add the request to the queue
//    QMutexLocker locker( & m_DB_RequestMutex );

//    pRequest = new iC3_DatabaseRequest( eDB_REQUEST_GET_ESIN_RANGE_FOR_CSV_FILES );

//    if ( pRequest != NULL )
//    {
//        pRequest->setTransactionID( uiTransactionID );
//        pRequest->setTransducerID( uiTransducerID );
//        pRequest->setEndSequenceIndex( ulLastESIN );
//        pRequest->setDownloadDataSelection( eSelection );

//        bRC = m_RequestProcessor.AddRequestToQueue(pRequest);
//    }

//    return bRC;
//}

////-----------------------------------------------------------------------------------------------
///** handleGraphEpochData() - SLOT that handles graph epoch request status in response to a
//*                            queued database request.
//*   @param uiTransactionID - unique identifier that was specified when the request was queued so
//*                            that the receiving logic knows which transaction ended successfully.
//*   @retval none
//*   @author Doug Sanqunetti
//*   @date 12/12/2013
//*   @note This message is passed through by emitting the signalGraphEpochData() signal
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::handleGraphEpochData( uint uiTransactionID )
//{
//    // forward signal to objects that have a connection to the database
//    emit signalGraphEpochData( uiTransactionID);
//}

////-----------------------------------------------------------------------------------------------
///** handleGraphDoorOpenData() - SLOT that handles graph door open data request status in
//*                            response to a queued database request.
//*   @param uiTransactionID - unique identifier that was specified when the request was queued so
//*                            that the receiving logic knows which transaction ended successfully.
//*   @retval none
//*   @author Doug Sanqunetti
//*   @date 12/15/2013
//*   @note This message is passed through by emitting the signalGraphDoorOpenData() signal
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::handleGraphDoorOpenData( uint uiTransactionID )
//{
//    // forward signal to objects that have a connection to the database
//    emit signalGraphDoorOpenData( uiTransactionID);
//}

////-----------------------------------------------------------------------------------------------
///** setInitialAlarmLimits() - Sets the initial upper and lower alarm limits for graphing
//*   @param uiTransducerID - unique identifier that specifies the requested Transducer
//*   @param fLowerAlarmLimit - the initial lower alarm limit
//*   @param fUpperAlarmLimit - the initial upper alarm limit
//*   @retval none
//*   @author Doug Sanqunetti
//*   @date 12/18/2013
//*   @note This function is called directly because it does not require database table access
//*         and should return immediately
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::setInitialAlarmLimits( uint uiTransducerID, float fLowerAlarmLimit, float fUpperAlarmLimit )
//{
//    m_RequestProcessor.setInitialAlarmLimits( uiTransducerID, fLowerAlarmLimit, fUpperAlarmLimit );
//}

////-----------------------------------------------------------------------------------------------
///** handleCommError() - SLOT that is notified if a Comm error occurrs
//*   @param eCommErrorLevel - an enumerated value that indicates a comm2 or comm3 error.  This
//*                            is not currently used by this function because in both cases we
//*                            need to close the database in preparation for rebooting.
//*   @retval none
//*   @author Doug Sanqunetti
//*   @date 03/17/2014
//*/
////-----------------------------------------------------------------------------------------------
//void iC3_Database::handleCommError( eDMM_CommErrorLevels eCommErrorLevel )
//{
//    Q_UNUSED( eCommErrorLevel );

//    closeDatabase();
//}

////-----------------------------------------------------------------------------------------------
///** commErrorMoveDatabase() - This function is called at DMM startup if a comm error forced a
//*                            reboot.  This function should move the existing database file to
//*                            new name so that we can create a new production database.
//*   @retval true - The database was moved
//*   @retval false - the database didn't exist or could not be moved.
//*   @author Doug Sanqunetti
//*   @date 03/19/2014
//*/
////-----------------------------------------------------------------------------------------------
//bool iC3_Database::commErrorMoveDatabase( void )
//{
//    bool bRC = false;
//    QString sDestDatabaseFileName;
//    QString sDateTimeString;
//    QString sDateTimeFormat = "MM-dd-yy_hh.mm.ss";

//    if ( QFile::exists(m_sDatabaseFileName) == true)
//    {
//        sDateTimeString = QDateTime::currentDateTime().toString(sDateTimeFormat);
//        sDestDatabaseFileName = QString("%1Corrupt_HelmerDB_%2").arg(HELMER_DATA_FILE_PATH).arg(sDateTimeString);

//        if ( QFile::copy(m_sDatabaseFileName, sDestDatabaseFileName ) == true )
//        {
//            bRC = QFile::remove( m_sDatabaseFileName );
//        }
//    }

//    return bRC;
//}
