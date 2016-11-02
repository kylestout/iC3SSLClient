/**
*     @file iC3_DatabaseTable.cpp
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief This cpp file implements the iC3_DatabaseTable class.  This class is
*            the base class of the Database Table clsses.
*/

#include "iC3_DatabaseTable.h"
#include <QDebug>
#include <QSqlError>

//-----------------------------------------------------------------------------------------------
/** constructor -  default constructor
*   @param parent (unused)
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DatabaseTable::iC3_DatabaseTable(QObject *parent) :
    QObject(parent)
{
}

//-----------------------------------------------------------------------------------------------
/** destructor -  default destructor - clears all column definitions
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DatabaseTable::~iC3_DatabaseTable()
{
    if ( m_qlColumnDefinitions.isEmpty() == false )
    {
        qDeleteAll(m_qlColumnDefinitions.begin(), m_qlColumnDefinitions.end());
        m_qlColumnDefinitions.clear();
    }
}

//-----------------------------------------------------------------------------------------------
/** AddColumnDef() -  adds a column definition to the list of column definitions
*   @param iIndex - column index
*   @param pColumnDef - pointer to a iC3_DatabaseColumnDef object defining the column
*   @retval true - the column definition was added
*   @retval false - the pColumDef argument was NULL
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseTable::AddColumnDef( int iIndex, iC3_DatabaseColumnDef *pColumnDef )
{
    if ( pColumnDef != NULL )
    {
        m_qlColumnDefinitions.insert(iIndex, pColumnDef);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------------------------
/** getColumnDef() -  returns a column definition from the list of column definitions
*   @param iColumnIndex - column index
*   @retval pointer to a iC3_DatabaseColumnDef object defining the column
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DatabaseColumnDef * iC3_DatabaseTable::getColumnDef( int iColumnIndex )
{
    iC3_DatabaseColumnDef   * pColumn;
    pColumn =  m_qlColumnDefinitions.at( iColumnIndex );

    return pColumn;
}

//-----------------------------------------------------------------------------------------------
/** setNumberOfColumns() -  specifies the number of columns in the table resrving space in the
*                           list of column definitions
*   @param iNumberOfColumns - number of columns in the table
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_DatabaseTable::setNumberOfColumns( int iNumberOfColumns )
{
    m_qlColumnDefinitions.reserve( iNumberOfColumns );
}

//-----------------------------------------------------------------------------------------------
/** getTableCreationSQL() -  returns the SQL required to create the table.  This is based upon
*                            columns defined using the AddColumnDef() function
*   @param sTableName - the Name of the table to be used in the table creation SQL
*   @retval SQL statement that will be used to create the table
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::getTableCreationSQL( QString sTableName )
{
    int iIndex;
    iC3_DatabaseColumnDef * pColumn;
    QString column;
    QString tableCreationSQL = QString("CREATE TABLE IF NOT EXISTS %1 ( ").arg(sTableName);

    for( iIndex = 0; iIndex < m_qlColumnDefinitions.size()-1; iIndex++ )
    {
        pColumn = m_qlColumnDefinitions.at(iIndex);
        column = QString("%1 %2 %3, ").arg(pColumn->getColumnName()).arg(pColumn->getDataType()).arg(pColumn->getConstraints());
        tableCreationSQL.append(column);
    }

    pColumn = m_qlColumnDefinitions.at(iIndex);
    column = QString("%1 %2 %3 );").arg(pColumn->getColumnName()).arg(pColumn->getDataType()).arg(pColumn->getConstraints());
    tableCreationSQL.append(column);

    return tableCreationSQL;
}

//-----------------------------------------------------------------------------------------------
/** GetLastError() -  if an error occurrs in a database operation, use this function to retrieve
*                     an error message string
*   @retval Error Message String
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::GetLastError( void )
{
    return m_sLastError;
}

//-----------------------------------------------------------------------------------------------
/** SetLastError() -  if an error occurrs in a database operation, this function is used to store
*                     an error message string
*   @param sLastErrorMessage -  Error Message String
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_DatabaseTable::SetLastError(QString sLastErrorMessage )
{
    m_sLastError = sLastErrorMessage;
}

//-----------------------------------------------------------------------------------------------
/** ClearLastError() -  clears any error messages from previous operations
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_DatabaseTable::ClearLastError( void )
{
    m_sLastError.clear();
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatBoolean() -  converts a boolean value to either a "0" or "1" so that it may be used
*                          in an SQL statement
*   @retval "0" if the boolean argument was false
*   @retval "1" if the boolean argument was true
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatBoolean( bool bValue )
{
    if ( bValue == true )
    {
        return QString("1");
    }
    else
    {
        return QString("0");
    }
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatString() -  places single quotes around a string so that it may be used in an SQL
*                         statement
*   @param sValue - the string value to be converted
*   @retval the QString argument with single quotes so that it may be used in an SQL statement
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatString( QString sValue )
{
    return QString("\'%1\'").arg(sValue);
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatDouble() -  converts a double value into a string
*   @param dValue - the double value to be converted
*   @retval (QString) double argument converted to a string so that it may be used in an SQL
*           statement
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatDouble( double dValue )
{
    return QString::number( dValue );
}

QString iC3_DatabaseTable::SQL_FormatFloat( float fValue )
{
    return QString::number( fValue );
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatQDate() -  converts a QDate value into a string
*   @param theDate - the QDate value to be converted
*   @retval (QString) QDate argument converted to a string so that it may be used in an SQL
*           statement
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatQDate( QDate theDate )
{
    return QString("\'%1\'").arg(theDate.toString("yyyy-MM-dd"));
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatQTime() -  converts a QTime value into a string
*   @param theTime - the QTime value to be converted
*   @retval (QString) QTime argument converted to a string so that it may be used in an SQL
*           statement
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatQTime( QTime theTime )
{
    return QString("\'%1\'").arg(theTime.toString("hh:mm:ss"));
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatQDateTime() -  converts a QDateTime value into a string
*   @param theDateTime - the QDateTime value to be converted
*   @retval (QString) QDateTime argument converted to a string so that it may be used in an SQL
*           statement
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatQDateTime( QDateTime theDateTime )
{
    return QString("\'%1\'").arg( theDateTime.toString("yyyy-MM-dd hh:mm:ss"));
}

//-----------------------------------------------------------------------------------------------
/** SQL_FormatQDateQTime() -  converts a QDate and a QTime value into a string
*   @param theDate - the QDate value to be converted
*   @param theTime - the QTime value to be converted
*   @retval (QString) QDate and QTime argument converted to a string so that they may be used in
*           an SQL statement
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::SQL_FormatQDateQTime( QDate theDate, QTime theTime )
{
    return QString("\'%1 %2\'").arg( theDate.toString("yyyy-MM-dd")).arg( theTime.toString("hh:mm:ss"));
}

//-----------------------------------------------------------------------------------------------
/** get_CSV_FileColumnHeaders() -  returns the CSV file column headers for the table.  This is
*               based upon columns defined using the AddColumnDef() function
*   @retval (QString) column headers in CSV format
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::get_CSV_FileColumnHeaders( void )
{
    int iIndex;
    iC3_DatabaseColumnDef * pColumn;
    QString column;
    QString CSV_FileHeader;

    for( iIndex = 0; iIndex < m_qlColumnDefinitions.size()-1; iIndex++ )
    {
        pColumn = m_qlColumnDefinitions.at(iIndex);
        column = QString("%1, ").arg(pColumn->getColumnName());
        CSV_FileHeader.append(column);
    }

    pColumn = m_qlColumnDefinitions.at(iIndex);
    column = QString("%1").arg(pColumn->getColumnName());
    CSV_FileHeader.append(column);

    return CSV_FileHeader;
}

//-----------------------------------------------------------------------------------------------
/** CheckTableIntegrity() -  attempts to select a single record from the table.
*   @retval true - the select statement was successful.  This can be true even if the table
*                  contains no rows.
*   @retval false - the select statement failed
*   @author  Doug Sanqunetti
*   @date 04/01/2014
*/
//-----------------------------------------------------------------------------------------------
bool iC3_DatabaseTable::CheckTableIntegrity( QSqlDatabase &database )
{
    bool bRC = true;
    QSqlQuery query( database );
    QString queryString;

    ClearLastError();

    if ( database.isOpen() )
    {
        queryString = QString( "SELECT * FROM %1 LIMIT 1").arg(m_sTableName);

        qDebug() << queryString;
        bRC = query.exec(queryString);

        if ( bRC == false )
        {
            QString sQueryError = query.lastError().text();
            SetLastError( QString("iC3_DatabaseTable::CheckTableIntegrity() - Query Error: %1").arg(sQueryError));
            qDebug() << m_sLastError;
            bRC = false;
        }
    }
    else
    {
        QString sErrorMessage = "iC3_DatabaseTable::CheckTableIntegrity() - Database is not open";
        SetLastError( sErrorMessage );
        qDebug() << m_sLastError;
        bRC = false;
    }

    return bRC;
}

//-----------------------------------------------------------------------------------------------
/** FormatDateString() -  formats the date retrieved from the database (variant) into a date
*                         string using the specified date format
*   @param  variantDate - the date returned from a database query as a vQVariant
*   @param  dateFormat - enumerated value indicating MMDDYY or DDMMYY
*   @retval the date in the requested format
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see  see the iC3_GUI_CONSTANTS for the definition of eDateFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::FormatDateString( QVariant variantDate, eDateFormats dateFormat )
{
    QString sReturnString;
    QString sDateFormatString;
    QDate theDate = variantDate.toDate();

    sDateFormatString = iC3_DMM_UtilityFunctions::convertToDateFormatString( dateFormat );
    sReturnString = theDate.toString( sDateFormatString );

    return sReturnString;
}

//-----------------------------------------------------------------------------------------------
/** FormatTimeString() -  formats the time retrieved from the database (variant) into a time
*                         string using the specified time format
*   @param  variantTime - the time returned from a database query as a vQVariant
*   @param  timeFormat - enumerated value indicating 12 or 24 hour format
*   @retval the time in the requested format
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see  see the iC3_GUI_CONSTANTS for the definition of eTimeFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::FormatTimeString( QVariant variantTime, eTimeFormats timeFormat )
{
    QString sReturnString;
    QString sTimeFormatString;
    QTime theTime = variantTime.toTime();

    sTimeFormatString = iC3_DMM_UtilityFunctions::convertToTimeFormatString(timeFormat);

    sReturnString = theTime.toString( sTimeFormatString );

    return sReturnString;
}

//-----------------------------------------------------------------------------------------------
/** FormatDateTimeString() -  formats the date/time retrieved from the database (variant) into a
*                         date/time string using the specified date and time format
*   @param  variantDateTime - the date/time returned from a database query as a vQVariant
*   @param  dateFormat - enumerated value indicating MMDDYY or DDMMYY
*   @param  timeFormat - enumerated value indicating 12 or 24 hour format
*   @retval the date/time in the requested format
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see  see the iC3_GUI_CONSTANTS for the definition of eDateFormats and eTimeFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseTable::FormatDateTimeString( QVariant variantDateTime, eDateFormats dateFormat, eTimeFormats timeFormat )
{
    QString sReturnString;
    QString sDateFormatString;
    QString sTimeFormatString;
    QString sDateString;
    QString sTimeString;

    QDateTime dateTime;

    dateTime = variantDateTime.toDateTime();

    sTimeFormatString = iC3_DMM_UtilityFunctions::convertToTimeFormatString( timeFormat );
    sDateFormatString = iC3_DMM_UtilityFunctions::convertToDateFormatString( dateFormat );

    sDateString = dateTime.toString( sDateFormatString );
    sTimeString = dateTime.toString( sTimeFormatString );

    sReturnString = QString("%1 %2").arg(sDateString).arg(sTimeString);

    return sReturnString;
}
