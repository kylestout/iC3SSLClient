#ifndef IC3_DATABASETABLE_H
#define IC3_DATABASETABLE_H

/**
*     @file iC3_DatabaseTable.h
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief This header file defines the iC3_DatabaseTable class.  This class is
*            the base class of the Database Table clsses.
*/

#include <QObject>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QVariant>

#include "iC3_DatabaseColumnDef.h"
#include "iC3_DMM_Constants.h"
#include "iC3_DMM_UtilityFunctions.h"

class iC3_DatabaseTable : public QObject
{
    Q_OBJECT
public:
    explicit iC3_DatabaseTable(QObject *parent = 0);
    ~iC3_DatabaseTable();

    bool AddColumnDef( int iIndex, iC3_DatabaseColumnDef * pColumnDef );
    iC3_DatabaseColumnDef * getColumnDef( int iColumnIndex );

    void setNumberOfColumns( int iNumberOfColumns );

    QVector<iC3_DatabaseColumnDef *> m_qlColumnDefinitions;

    virtual QString getTableCreationSQL( QString sTableName );

    QString GetLastError( void );
    virtual void SetLastError(QString sLastErrorMessage );
    void ClearLastError( void );

    QString get_CSV_FileColumnHeaders( void );

    QString SQL_FormatBoolean( bool bValue );
    QString SQL_FormatDouble( double dValue );
    QString SQL_FormatFloat( float fValue );
    QString SQL_FormatString( QString sValue);
    QString SQL_FormatQDate( QDate theDate );
    QString SQL_FormatQTime( QTime theTime );
    QString SQL_FormatQDateTime( QDateTime theDateTime );
    QString SQL_FormatQDateQTime( QDate theDate, QTime theTime );

    bool CheckTableIntegrity( QSqlDatabase &database );

    QString FormatDateString( QVariant variantDate, eDateFormats dateFormat );
    QString FormatTimeString( QVariant variantTime, eTimeFormats timeFormat );
    QString FormatDateTimeString( QVariant variantDateTime, eDateFormats dateFormat, eTimeFormats timeFormat );


protected:

    QString m_sTableName;
    QString m_sLastError;

private:

};

#endif // IC3_DATABASETABLE_H
