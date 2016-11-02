#ifndef IC3_TRANSDUCERTABLE_H
#define IC3_TRANSDUCERTABLE_H

#include <QFile>
#include "iC3_DatabaseTable.h"

class iC3_DatabaseAccessLogData;
class iC3_AccessLogData;

class iC3_TransducerTable : public iC3_DatabaseTable
{
public:
    iC3_TransducerTable();

    bool insertNewEntry( QSqlDatabase & database,
                         double fRTD1Val,
                         double fRTD2Val,
                         double fRTD3Val,
                         double fRTD4Val,
                         double fRTD5Val );

    bool getAccessLogEntry( QSqlDatabase & database,
                            quint32 ulEventSequenceIndex,
                            iC3_AccessLogData * pAccessLogEntry );

    bool updateAccessLogEntry( QSqlDatabase & database,
                               iC3_AccessLogData * pAccessLogEntry );

    bool getLastMultAccessLogData( QSqlDatabase & database, unsigned int uiNumberOfEvents, iC3_DatabaseAccessLogData *pAccessLogData );


    enum eIC3_TransducerTableColumns
    {
        e_TRANSDUCER_TABLE_DATE_TIME_COL            = 0,
        e_TRANSDUCER_TABLE_RTD_1_TEMP_COL           = 1,
        e_TRANSDUCER_TABLE_RTD_2_TEMP_COL           = 2,
        e_TRANSDUCER_TABLE_RTD_3_TEMP_COL           = 3,
        e_TRANSDUCER_TABLE_RTD_4_TEMP_COL           = 4,
        e_TRANSDUCER_TABLE_RTD_5_TEMP_COL           = 5,

        e_NUMBER_OF_TRANSDUCER_TABLE_COLUMNS
    };

    QString getTableCreationSQL( void );

    bool CreateTable( QSqlDatabase & database );

    QString getSQL_ColumnNames( void );

    QString getSQL_ColumnValues( iC3_AccessLogData *pAccessLogEntry );

    bool updateAccessLogDataFromQuery( iC3_AccessLogData * pAccessLogEntry, QSqlQuery & query );

private:

};

#endif // IC3_TRANSDUCERTABLE_H
