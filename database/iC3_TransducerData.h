#ifndef IC3_ACCESSLOGDATA_H
#define IC3_ACCESSLOGDATA_H

/**
*     @file iC3_TransducerData.h
*     @author Doug Sanqunetti
*     @date 01/27/2013
*     @version 1.0
*     @brief this header file defines the iC3_TransducerData class.  This class represents the data
*            contained in the access log for a single event.
*/
#include <QVariant>
#include <QObject>
#include <QDate>
#include <QTime>
#include <QColor>
#include <QFont>

#include "iC3_GUI_Constants.h"

class iC3_TransducerData : public QObject
{
    Q_OBJECT
public:
     iC3_TransducerData();

     iC3_TransducerData( iC3_TransducerData & SourceLog );

     iC3_TransducerData( quint32 uiEventSequenceIndex,
                        int accessEvent,
                        QString sUser,
                        QDate accessDate,
                        QTime accessTime,
                        int durationMinutes,
                        int eAccessMethod );

     iC3_TransducerData * operator =( iC3_TransducerData & SourceLog );

    void    setAccessEvent( int accessEvent );
    eAccessEvents getAccessEvent( void );

    void    setUser( QString sUser );
    QString getUser( void );

    void  setAccessDate( QDate accessDate );
    QDate getAccessDate( void );

    void  setAccessTime( QTime accessTime);
    QTime getAccessTime( void );

    void  setDurationMinutes( int iDurationMinutes );
    QVariant getDurationMinutes( void );
    int   getIntDurationMinutes( void );

    void     setAccessMethod( int eAccessMethod );
    eAccessMethods getAccessMethodEnum( void );
    QString  getAccessMethod( void );

    quint32 getEventSequenceIndex( void );
    void    setEventSequenceIndex( quint32 ulEventSequenceIndex );

    QFont getAccessEventFont(void);
    void  setAccessEventFont( QFont newFont );

    bool   getAccessEventViewed( void );
    void   setAccessEventViewed( bool bViewed );

    bool   getAccessEventDownloaded( void );
    void   setAccessEventDownloaded( bool bDownloaded );

private:

    bool   m_bDownloaded;
    bool   m_bViewed;
    eAccessEvents m_eAccessEvent;
    QString m_sUser;                      // 4.22.1.1
    QDate m_accessDate;                   // 4.22.1.2
    QTime m_accessTime;                   // 4.22.1.3
    int   m_iDurationMinutes;             // 4.22.1.4
    eAccessMethods m_eAccessMethod;       // 4.22.1.5

    quint32 m_uiEventSequenceIndex;
};
#endif // iC3_TransducerData_H
