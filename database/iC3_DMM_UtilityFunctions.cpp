/**
*     @file iC3_DMM_UtilityFunctions.cpp
*     @author Doug Sanqunetti
*     @date 09/01/2013
*     @version 1.0
*     @brief this cpp file implements the iC3_DMM_UtilityFunctions class.  This class is
*           used to contain static functions that are used in multiple classes within
*           the data management module.
*/

#include "iC3_DMM_UtilityFunctions.h"

//-----------------------------------------------------------------------------------------------
/** constructor
*   @param parent - QObject pointer parent (unused)
*   @retval none
*   @author  Doug Sanqunetti
*   @date 09/01/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_DMM_UtilityFunctions::iC3_DMM_UtilityFunctions(QObject *parent) :
    QObject(parent)
{
}

//-----------------------------------------------------------------------------------------------
/** getTransducerTypeString() - returns a string that represents the transducer type
*   @param eTransducerType - an enumerated transducer type
*   @return Transducer Type String
*   @author  Doug Sanqunetti
*   @date 01/19/2013
*   @see iC3_DMM_Constants.h for the definition of eTransducerType
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DMM_UtilityFunctions::getTransducerTypeString( eIC3_TransducerType eTransducerType )
{
    switch( eTransducerType )
    {
    case eTEMPERATURE_TRANSDUCER:
        return tr("Temp");
        break;

    case eVOLTAGE_TRANSDUCER:
        return tr("Volt");
        break;

    case eHUMIDITY_TRANSDUCER:
        return tr("Humidity");
        break;

    case eCURRENT_TRANSDUCER:
        return tr("Current");
        break;

    case ePRESSURE_TRANSDUCER:
        return tr("Pressure");
        break;

    case eDIGITAL_TRANSDUCER:
        return tr("Digital");
        break;

    case eLUMENS_TRANSDUCER:
        return tr("Lumens");
        break;

    case eDB_TRANSDUCER:
        return tr("db");
        break;

    case eLOADCELL_TRANSDUCER:
        return tr("Loadcell");
        break;

    case eACCELERATION_TRANSDUCER:
        return tr("Accel");
        break;

    case eTILT_TRANSDUCER:
        return tr("Tilt");
        break;

    case eFREQUENCY_TRANSDUCER:
        return tr("Frequency");
        break;

    case eRESISTANCE_TRANSDUCER:
        return tr("Resistance");
        break;

    default:
        return tr("Unknown");
        break;
    }
}

//-----------------------------------------------------------------------------------------------
/** convertToTimeFormatString() - converts an enumerated eTimeFormats argument into a time format
*                                 string;
*   @param timeFormat - an enumerated time format
*   @return  time format string
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see iC3_GUI_Constants.h for the definition of eTimeFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DMM_UtilityFunctions::convertToTimeFormatString( eTimeFormats timeFormat )
{
    QString sTimeFormatString;

    switch( timeFormat )
    {
    case eTIME_FORMAT_12_HOUR:
        sTimeFormatString = "h:mm ap";
        break;
    case eTIME_FORMAT_24_HOUR:
        sTimeFormatString = "hh:mm";
        break;
    default:
        sTimeFormatString = "hh:mm";
        break;
    }

    return sTimeFormatString;
}

//-----------------------------------------------------------------------------------------------
/** convertToDateFormatString() - converts an enumerated eDateFormats argument into a date format
*                                 string;
*   @param dateFormat - an enumerated date format
*   @return  date format string
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see iC3_GUI_Constants.h for the definition of eDateFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DMM_UtilityFunctions::convertToDateFormatString( eDateFormats dateFormat )
{
    QString sDateFormatString;

    switch( dateFormat )
    {
    case eDATE_FORMAT_MMDDYY:
        sDateFormatString = "MM/dd/yyyy";
        break;
    case eDATE_FORMAT_DDMMYY:
        sDateFormatString = "dd/MM/yyyy";
        break;
    }

    return sDateFormatString;
}

//-----------------------------------------------------------------------------------------------
/** convertToFilePathTimeFormatString() - converts an enumerated eTimeFormats argument into a
*                                 time format string that can be used as part of a file path;
*   @param timeFormat - an enumerated time format
*   @return  time format string
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see iC3_GUI_Constants.h for the definition of eTimeFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DMM_UtilityFunctions::convertToFilePathTimeFormatString( eTimeFormats timeFormat )
{
    Q_UNUSED(timeFormat);

    QString sTimeFormatString;

//    switch( timeFormat )
//    {
//    case eTIME_FORMAT_12_HOUR:
//    case eTIME_FORMAT_24_HOUR:
//    default:
        sTimeFormatString = "hh.mm.ss";
//        break;
//    }

    return sTimeFormatString;
}

//-----------------------------------------------------------------------------------------------
/** convertToFilePathDateFormatString() - converts an enumerated eDateFormats argument into a
*                                 date format string that can be used as part of a file path;
*   @param dateFormat - an enumerated date format
*   @return  date format string
*   @author  Doug Sanqunetti
*   @date 04/17/2014
*   @see iC3_GUI_Constants.h for the definition of eDateFormats
*/
//-----------------------------------------------------------------------------------------------
QString iC3_DMM_UtilityFunctions::convertToFilePathDateFormatString( eDateFormats dateFormat )
{
    QString sDateFormatString;

    switch( dateFormat )
    {
    case eDATE_FORMAT_MMDDYY:
        sDateFormatString = "MM-dd-yyyy";
        break;
    case eDATE_FORMAT_DDMMYY:
        sDateFormatString = "dd-MM-yyyy";
        break;
    }

    return sDateFormatString;
}
