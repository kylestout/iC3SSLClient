#ifndef IC3_DMM_UTILITYFUNCTIONS_H
#define IC3_DMM_UTILITYFUNCTIONS_H
/**
*     @file iC3_DMM_UtilityFunctions.h
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief this header file defines the iC3_DMM_UtilityFunctions class.  This class is
*           used to contain static functions that are used in multiple classes within
*           the data management module.
*/

#include <QObject>
#include <string>

#include "iC3_DMM_Constants.h"

class iC3_DMM_UtilityFunctions : public QObject
{
    Q_OBJECT
public:
    explicit iC3_DMM_UtilityFunctions(QObject *parent = 0);

    static QString getTransducerTypeString( eIC3_TransducerType eTransducerType );  
    static QString convertToTimeFormatString( eTimeFormats timeFormat );
    static QString convertToDateFormatString( eDateFormats dateFormat );
    static QString convertToFilePathTimeFormatString( eTimeFormats timeFormat );
    static QString convertToFilePathDateFormatString( eDateFormats dateFormat );
};

#endif // IC3_DMM_UTILITYFUNCTIONS_H
