#ifndef CALIBRATIONMANAGER_H
#define CALIBRATIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

static const double TEMPERATURE_SETPOINT_4C_REFRIGERATOR = 4.0;
static const double TEMPERATURE_SETPOINT_NEG_30C_FREEZER = -30.0;
static const double STABLE_TEMPERATURE_DELTA_UPPER_BOUND = 0.15;
static const double STABLE_TEMPERATURE_DELTA_LOWER_BOUND = -0.15;
static const double CALIBRATED_TEMPERATURE_BUFFER = 0.25;
static const double OFFSET_VALUE_MAX = 10.0;
static const double OFFSET_VALUE_MIN = -10.0;

static const int COMPRESSOR_CYCLES_CHECK = 6;
static const int ONE_SECOND = 1000;
static const int ONE_MINUTE = 60000;
static const int FIFTEEN_MINUTES = 900000;

enum eCalibrationStates
{
    eCALIBRATION_STATE_TEMPERATURE_UNSTABLE = 0,
    eCALIBRATION_STATE_TEMPERATURE_STABLE   = 1,
    eCALIBRATION_STATE_CALIBRATED           = 2
};

struct cycleData
{
    QDateTime dtDateTimeStart;
    QDateTime dtDateTimeEnd;
    int iCycleNumber;
    double dTempStart;
    double dTempEnd;
};

class Client;

class CalibrationManager : public QObject
{
    Q_OBJECT

public:
    CalibrationManager(Client *pClient,
                       QObject *parent = 0);

    ~CalibrationManager();

    eCalibrationStates getCalibrationState();
    void setCalibrationState(eCalibrationStates eState);
    void fixPrimaryProbeOffset();
    void fixControlProbeOffset();
    void compressorCycleStart();

public slots:
    void    slot_FifteenMinuteTimeout();
    void    slot_UpdateTemperatureValuesTimeout();
    void    slot_CompressorStateCheckTimeout();

signals:


private:
    Client* m_pClient;

    QList<cycleData> m_CycleDataList;

    QTimer  m_tFifteenMinuteTimer;
    QTimer  m_tUpdateTemperatureValuesTimer;
    QTimer  m_tCompressorStateCheckTimer;

    int     m_iNumberCompressorCycles;

//    double  m_dPrimaryTemperatureDelta;
    double  m_dPrimaryTemperature;
    double  m_dControlTemperature;
    double  m_dFlukeChannel1Temperature;
    double  m_dRTD4_OffsetValue;
    double  m_dRTD5_OffsetValue;

    bool    m_bCompressorState;
    eCalibrationStates  m_eCalibrationState;

};

#endif // CALIBRATIONMANAGER_H
