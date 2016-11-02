#ifndef IC3_DATABASECOLUMNDEF_H
#define IC3_DATABASECOLUMNDEF_H

/**
*     @file iC3_DatabaseColumnDef.h
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief This header file defines the iC3_DatabaseColumnDef class.  This class contains
*            all of the information necessary to define a Database Table column
*/

#include <QObject>

class iC3_DatabaseColumnDef : public QObject
{
    Q_OBJECT
public:
    explicit iC3_DatabaseColumnDef(QObject *parent = 0);
    iC3_DatabaseColumnDef( QString sColumnName,
                           const char *sDataType,
                           const char *sConstraints,
                           QObject *parent = 0 );

    QString getColumnName( void );
    void    setColumnName( QString & sColumnName );

    QString getDataType( void );
    void    setDataType( QString & sDataType );

    QString getConstraints( void );
    void    setConStraints( QString & sConstraints );

private:

    QString m_sColumnName;
    QString m_sDataType;
    QString m_sConstraints;
};

#endif // IC3_DATABASECOLUMNDEF_H
