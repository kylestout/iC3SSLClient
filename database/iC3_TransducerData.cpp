/**
*     @file iC3_TransducerData.cpp
*     @author Doug Sanqunetti
*     @date 01/27/2013
*     @version 1.0
*     @brief this cpp file implements the iC3_TransducerData class.  This class represents the data
*            contained in the access log for a single event.
*/

#include "iC3_TransducerData.h"

iC3_TransducerData::iC3_TransducerData():
    QObject(0),
    m_bDownloaded(false),
    m_bViewed( false ),
    m_eAccessEvent(eACCESS_EVENT_UNKNOWN),
    m_iDurationMinutes(0),
    m_eAccessMethod(eACCESS_METHOD_UNKNOWN),
    m_uiEventSequenceIndex(0)
{
    m_sUser.clear();
    m_accessDate.fromJulianDay(0);
    m_accessTime = QTime::fromString("00:00:01","hh:mm:ss");
}

//-----------------------------------------------------------------------------------------------
/** constructor
*   @param uiEventSequenceIndex - unique number indicating the occurrence of the data point with
*          respect to the Events Sequence
*   @param accessEvent - enumerated event type eACCESS_EVENT_NORMAL_ACCESS,
*          eACCESS_EVENT_DENIED_ACCESS or eACCESS_EVENT_OVERRIDE_ACCESS
*   @param sUser - User Name if the accessEvent == eACCESS_EVENT_NORMAL_ACCESS
*                  blank if the accessEvent == eACCESS_EVENT_DENIED_ACCESS
*                  blank if the accessEvent == eACCESS_EVENT_OVERRIDE_ACCESS
*                  4.22.1.1
*   @param accessDate - 4.22.1.2 date of access
*   @param accessTime - 4.22.1.3 time of access
*   @param durationMinutes - 4.22.1.4 length of time unit was opened
*   @param eAccessMethod -  4.22.1.5 enumerated value - How unit was accessed.
*                           eACCESS_METHOD_PIN, eACCESS_METHOD_KEY, eACCESS_METHOD_PROX_CARD OR
*                           eACCESS_METHOD_MAGNETIC_SWIPE
*   @retval none
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_TransducerData::iC3_TransducerData( quint32 uiEventSequenceIndex,
                                      int accessEvent,
                                      QString sUser,
                                      QDate accessDate,
                                      QTime accessTime,
                                      int durationMinutes,
                                      int eAccessMethod ):
    QObject(0)
{
    m_uiEventSequenceIndex = uiEventSequenceIndex;
    m_sUser = sUser;
    m_accessDate = accessDate;
    m_accessTime = accessTime;
    m_iDurationMinutes = durationMinutes;
    m_eAccessMethod = (eAccessMethods) eAccessMethod;
    m_bDownloaded = false;
    m_bViewed = false;

    QDateTime accessDateTime;
    accessDateTime.setDate( m_accessDate );
    accessDateTime.setTime( m_accessTime );

    setAccessEvent( accessEvent );
}

//-----------------------------------------------------------------------------------------------
/** Copy Constructor
*   @param SourceLog - Log To be Copied
*   @retval none
*   @author  Doug Sanqunetti
*   @date 01/19/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_TransducerData::iC3_TransducerData( iC3_TransducerData & SourceLog ) :
    QObject(0)
{
    m_bDownloaded          = SourceLog.m_bDownloaded;
    m_bViewed              = SourceLog.m_bViewed;
    m_eAccessEvent         = SourceLog.m_eAccessEvent;
    m_sUser                = SourceLog.m_sUser;
    m_accessDate           = SourceLog.m_accessDate;
    m_accessTime           = SourceLog.m_accessTime;
    m_iDurationMinutes     = SourceLog.m_iDurationMinutes;
    m_eAccessMethod        = SourceLog.m_eAccessMethod;
    m_uiEventSequenceIndex = SourceLog.m_uiEventSequenceIndex;
}

//-----------------------------------------------------------------------------------------------
/** operator=
*   @param SourceLog - Log To be Copied
*   @retval none
*   @author  Doug Sanqunetti
*   @date 01/19/2013
*/
//-----------------------------------------------------------------------------------------------
iC3_TransducerData * iC3_TransducerData::operator =( iC3_TransducerData & SourceLog )
{
    m_bDownloaded          = SourceLog.m_bDownloaded;
    m_bViewed              = SourceLog.m_bViewed;
    m_eAccessEvent         = SourceLog.m_eAccessEvent;
    m_sUser                = SourceLog.m_sUser;
    m_accessDate           = SourceLog.m_accessDate;
    m_accessTime           = SourceLog.m_accessTime;
    m_iDurationMinutes     = SourceLog.m_iDurationMinutes;
    m_eAccessMethod        = SourceLog.m_eAccessMethod;
    m_uiEventSequenceIndex = SourceLog.m_uiEventSequenceIndex;

    return this;
}

//-----------------------------------------------------------------------------------------------
/** setAccessEvent() - sets the Access Event.
*   @param accessEvent - enumerated value eACCESS_EVENT_NORMAL_ACCESS,
*                        eACCESS_EVENT_DENIED_ACCESS or eACCESS_EVENT_OVERRIDE_ACCESS
*   @return void
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_TransducerData::setAccessEvent( int accessEvent )
{
    m_eAccessEvent = (eAccessEvents) accessEvent;
}

//-----------------------------------------------------------------------------------------------
/** getAccessEvent() - returns the enumerated Access Event.
*   @return accessEvent - enumerated value eACCESS_EVENT_NORMAL_ACCESS,
*                        eACCESS_EVENT_DENIED_ACCESS or eACCESS_EVENT_OVERRIDE_ACCESS
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
eAccessEvents iC3_TransducerData::getAccessEvent( void )
{
    return m_eAccessEvent;
}

//-----------------------------------------------------------------------------------------------
/** setUser() - sets the Access Log User field 4.22.1.1
*   @param sUser - the User Name string
*   @return void
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_TransducerData::setUser( QString sUser )
{
    m_sUser = sUser;
}

//-----------------------------------------------------------------------------------------------
/** getUser() - gets the Access Log User field 4.22.1.1
*   @return if the accessEvent is eACCESS_EVENT_NORMAL_ACCESS the user Name is returned
*   @return if the accessEvent is eACCESS_EVENT_DENIED_ACCESS the string "Denied" is returned
*   @return if the accessEvent is eACCESS_EVENT_OVERRIDE_ACCESS the string "Override" is returned
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*   @see  constructor, setAccessEvent()
*/
//-----------------------------------------------------------------------------------------------
QString iC3_TransducerData::getUser( void )
{
    switch( m_eAccessEvent )
    {
    case eACCESS_EVENT_NORMAL_ACCESS:
        return m_sUser;
        break;
    case eACCESS_EVENT_DENIED_ACCESS:
        return QString(tr("Denied"));
        break;
    case eACCESS_EVENT_OVERRIDE_ACCESS:
        return QString(tr("Override"));
        break;
    default:
        return QString("");
        break;
    }
}

//-----------------------------------------------------------------------------------------------
/** setAccessDate() - sets the Access Date field 4.22.1.2
*   @param accessDate - the Access Date (QDate)
*   @return void
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void  iC3_TransducerData::setAccessDate( QDate accessDate )
{
    m_accessDate = accessDate;
}

//-----------------------------------------------------------------------------------------------
/** getAccessDate() - gets the Access Date field 4.22.1.2
*   @return the Access Date (QDate)
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
QDate iC3_TransducerData::getAccessDate( void )
{
    return m_accessDate;
}

//-----------------------------------------------------------------------------------------------
/** setAccessTime() - sets the Access Time field 4.22.1.3
*   @param accessTime - the Access Date (QTime)
*   @return void
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void  iC3_TransducerData::setAccessTime( QTime accessTime )
{
    m_accessTime = accessTime;
}

//-----------------------------------------------------------------------------------------------
/** getAccessTime() - gets the Access Time field 4.22.1.3
*   @return accessTime (QTime)
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
QTime iC3_TransducerData::getAccessTime( void )
{
    return m_accessTime;
}

//-----------------------------------------------------------------------------------------------
/** setDurationMinutes() - sets the Duration field 4.22.1.4 (Minutes)
*   @param iDurationMinutes - The length of time the unit was opened
*   @return void
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void  iC3_TransducerData::setDurationMinutes( int iDurationMinutes )
{
    m_iDurationMinutes = iDurationMinutes;
}

//-----------------------------------------------------------------------------------------------
/** getDurationMinutes() - gets the Duration field 4.22.1.4 (Minutes)
*   @return lenth of time the unit was opened (QVariant(int))
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
QVariant iC3_TransducerData::getDurationMinutes( void )
{
    return QVariant(m_iDurationMinutes);
}

//-----------------------------------------------------------------------------------------------
/** getIntDurationMinutes() - gets the Duration field 4.22.1.4 (Minutes)
*   @return lenth of time the unit was opened (int)
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
int iC3_TransducerData::getIntDurationMinutes( void )
{
    return m_iDurationMinutes;
}

//-----------------------------------------------------------------------------------------------
/** setAccessMethod() - sets the Method field 4.22.1.5  enumerated value
*   @param eAccessMethod - enumerated Access Method. eACCESS_METHOD_PIN , eACCESS_METHOD_KEY,
*                          eACCESS_METHOD_PROX_CARD or eACCESS_METHOD_MAGNETIC_SWIPE
*   @return void
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_TransducerData::setAccessMethod( int eAccessMethod )
{
    m_eAccessMethod = (eAccessMethods) eAccessMethod;
}

//-----------------------------------------------------------------------------------------------
/** getAccessMethodEnum() - gets the Method field 4.22.1.5  enumerated value
*   @return eAccessMethod - enumerated Access Method. eACCESS_METHOD_PIN , eACCESS_METHOD_KEY,
*                          eACCESS_METHOD_PROX_CARD or eACCESS_METHOD_MAGNETIC_SWIPE
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
eAccessMethods iC3_TransducerData::getAccessMethodEnum( void )
{
    return m_eAccessMethod;
}

//-----------------------------------------------------------------------------------------------
/** getAccessMethod() - gets the Method field 4.22.1.5  string value
*   @return if AccessMethod = eACCESS_METHOD_PIN returns "PIN" string
*   @return if AccessMethod = eACCESS_METHOD_KEY returns "Key" string
*   @return if AccessMethod = eACCESS_METHOD_PROX_CARD returns "Proxy Card" string
*   @return if AccessMethod = eACCESS_METHOD_MAGNETIC_SWIPE returns "Magnetic Swipe" string
*   @return if AccessMethod = eACCESS_METHOD_TEST_FUNTIONALITY returns "4.60 Test Functionality"
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
QString iC3_TransducerData::getAccessMethod( void )
{
    switch( m_eAccessMethod )
    {
    case eACCESS_METHOD_PIN:
        return QString(tr("PIN"));
        break;
    case eACCESS_METHOD_KEY:
        return QString(tr("Key"));
        break;
    case eACCESS_METHOD_PROX_CARD:
        return QString(tr("Prox Card"));
        break;
    case eACCESS_METHOD_MAGNETIC_SWIPE:
        return QString(tr("Magnetic Swipe"));
        break;
    case eACCESS_METHOD_TEST_FUNTIONALITY:
        return QString(tr("Tech Override"));
        break;
    default:
        return QString("");
        break;
    }
}
//-----------------------------------------------------------------------------------------------
/** getEventSequenceIndex() - returns the Event Sequence Index
*   @return Event Sequence Index - unique number indicating the occurrence of the data point with
*               respect to the Events Sequence
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
quint32 iC3_TransducerData::getEventSequenceIndex( void )
{
    return m_uiEventSequenceIndex;
}

//-----------------------------------------------------------------------------------------------
/** setEventSequenceIndex() - sets the Event Sequence Index
*   @param ulEventSequenceIndex - unique number indicating the occurrence of the data point with
*               respect to the Events Sequence
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_TransducerData::setEventSequenceIndex( quint32 ulEventSequenceIndex )
{
    m_uiEventSequenceIndex = ulEventSequenceIndex;
}
//-----------------------------------------------------------------------------------------------
/** getAccessEventViewed() - gets a boolean value indicating whether this event has been
*          viewed in the GUI
*   @return true if this event has been viewed
*   @return false if this event has not been viewed
*   @author  Doug Sanqunetti
*   @date 11/29/2013
*/
//-----------------------------------------------------------------------------------------------
bool   iC3_TransducerData::getAccessEventViewed( void )
{
    return m_bViewed;
}

//-----------------------------------------------------------------------------------------------
/** setAccessEventViewed() - sets a boolean value indicating whether this event has been
*          viewed.
*   @param bViewed - true if this event has been viewed
*                    false if this event has not been viewed
*   @author  Doug Sanqunetti
*   @date 11/29/2013
*/
//-----------------------------------------------------------------------------------------------
void iC3_TransducerData::setAccessEventViewed( bool bViewed )
{
    m_bViewed = bViewed;
}

//-----------------------------------------------------------------------------------------------
/** getAccessEventDownloaded() - gets a boolean value indicating whether this event has been
*          downloaded
*   @return true if this event has been downloaded
*   @return false if this event has not been downloaded
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
bool   iC3_TransducerData::getAccessEventDownloaded( void )
{
    return m_bDownloaded;
}

//-----------------------------------------------------------------------------------------------
/** setAccessEventDownloaded() - sets a boolean value indicating whether this event has been
*          downloaded. if the event has been downloaded (bDownloded argument=true) then the
*          AccessEventColor is set to QColor(Qt::Gray)
*   @param bDownloaded - true if this event has been downloaded
*                        false if this event has not been downloaded
*   @author  Doug Sanqunetti
*   @date 01/27/2013
*/
//-----------------------------------------------------------------------------------------------
void   iC3_TransducerData::setAccessEventDownloaded( bool bDownloaded )
{
    m_bDownloaded = bDownloaded;
}
