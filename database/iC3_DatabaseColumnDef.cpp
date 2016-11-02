/**
*     @file iC3_DatabaseColumnDef.cpp
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief This cpp file implements the iC3_DatabaseColumnDef class.  This class contains
*            all of the information necessary to define a Database Table column
*/

#include "iC3_DatabaseColumnDef.h"

//-----------------------------------------------------------------------------------------------
/** constructor - the default constructor
*   @param parent - QObject Pointer (unused)
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DatabaseColumnDef::iC3_DatabaseColumnDef(QObject *parent) :
    QObject(parent)
{
}

//-----------------------------------------------------------------------------------------------
/** constructor - constructor with arguments
*   @param sColumnName - the name of the column (QString)
*   @param sDataType - one of data types understood by SQLite ("INTEGER", "TEXT", "FLOAT", etc.)
*   @param sConstraints - column constrains understood by SQLite ("PRIMARY KEY AUTOINCREMENT", etc.)
*   @param parent - QObject Pointer (unused)
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DatabaseColumnDef::iC3_DatabaseColumnDef( QString sColumnName,
                                              const char * sDataType,
                                              const char * sConstraints,
                                              QObject *parent) :
    QObject(parent)
{
    m_sColumnName = sColumnName;
    m_sDataType = sDataType;
    m_sConstraints = sConstraints;
}

//-----------------------------------------------------------------------------------------------
/** getColumnName - Accessor function used to retrieve the column name
*   @retval column Name (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseColumnDef::getColumnName( void )
{
    return m_sColumnName;
}

//-----------------------------------------------------------------------------------------------
/** setColumnName - Mutator function used to set the column name
*   @param sColumnName - column Name (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void    iC3_DatabaseColumnDef::setColumnName( QString & sColumnName )
{
    m_sColumnName = sColumnName;
}

//-----------------------------------------------------------------------------------------------
/** getDataType - Accessor function used to retrieve the data type
*   @retval data type (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseColumnDef::getDataType( void )
{
    return m_sDataType;
}

//-----------------------------------------------------------------------------------------------
/** setDataType - Mutator function used to set the column data type
*   @param sDataType - data type - understood by SQLite ("INTEGER", "TEXT", "FLOAT", etc.)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void    iC3_DatabaseColumnDef::setDataType( QString & sDataType )
{
    m_sDataType = sDataType;
}

//-----------------------------------------------------------------------------------------------
/** getConstraints - Accessor function used to retrieve the column constraints
*   @retval column constraints (QString)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DatabaseColumnDef::getConstraints( void )
{
    return m_sConstraints;
}

//-----------------------------------------------------------------------------------------------
/** setConStraints - Mutator function used to set the column constraints
*   @param sConstraints - understood by SQLite ("PRIMARY KEY AUTOINCREMENT", etc.)
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
void    iC3_DatabaseColumnDef::setConStraints( QString & sConstraints )
{
    m_sConstraints = sConstraints;
}
