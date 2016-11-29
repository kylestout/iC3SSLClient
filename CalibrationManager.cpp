#include "CalibrationManager.h"
#include "client.h"

CalibrationManager::CalibrationManager(Client *pClient,
                                       QObject *parent) :
    m_pClient(pClient),
    QObject(parent),
    m_bCompressorState(false),
    m_dControlTemperature(100.0),
    m_dFlukeChannel1Temperature(100.0),
    m_dPrimaryTemperature(100.0),
    m_dRTD4_OffsetValue(0.0),
    m_dRTD5_OffsetValue(0.0),
    m_iNumberCompressorCycles(-1),
    m_eCalibrationState(eCALIBRATION_STATE_TEMPERATURE_UNSTABLE)
{
    connect(&m_tFifteenMinuteTimer, SIGNAL(timeout()), this, SLOT(slot_FifteenMinuteTimeout()));
    m_tFifteenMinuteTimer.start(FIFTEEN_MINUTES);

    connect(&m_tUpdateTemperatureValuesTimer, SIGNAL(timeout()), this, SLOT(slot_UpdateTemperatureValuesTimeout()));
    m_tUpdateTemperatureValuesTimer.start(ONE_MINUTE);
}

CalibrationManager::~CalibrationManager()
{

}

void CalibrationManager::slot_FifteenMinuteTimeout()
{
    double dNewPrimaryTemperature = m_pClient->getPrimaryTemp();
    double dPrimaryTemperatureDelta = dNewPrimaryTemperature - m_dPrimaryTemperature;  //calculate temp delta every 15 min
    m_dPrimaryTemperature = dNewPrimaryTemperature;

    qDebug() << "CalibrationManager::slot_FifteenMinuteTimeout()";

    if ( getCalibrationState() == eCALIBRATION_STATE_TEMPERATURE_UNSTABLE ) // check for temp stability during pulldown
    {
        if ( (dPrimaryTemperatureDelta >= STABLE_TEMPERATURE_DELTA_LOWER_BOUND) &&
             (dPrimaryTemperatureDelta <= STABLE_TEMPERATURE_DELTA_UPPER_BOUND) )  // temp must not change beyond 0.1 C within 15 minutes to constitute stability
        {
            qDebug() << "{ Temperature is now stable! } -> Temperature Delta: " << dPrimaryTemperatureDelta;
            setCalibrationState(eCALIBRATION_STATE_TEMPERATURE_STABLE);

            fixControlProbeOffset();

            return;
        }
        else
        {
            qDebug() << " >>>> Temperature is not yet stable -> Temperature Delta: " << dPrimaryTemperatureDelta;
        }
    }
    else if ( getCalibrationState() == eCALIBRATION_STATE_TEMPERATURE_STABLE )
    {
        qDebug() << "{ Temperature is stable } -> Temperature Delta: " << dPrimaryTemperatureDelta;

        fixPrimaryProbeOffset();
    }
    else if ( getCalibrationState() == eCALIBRATION_STATE_CALIBRATED )
    {
        qDebug() << " ******** UNIT IS CALIBRATED ********";
    }
    else
    {
        qWarning() << "ERROR! -> Temperature Delta: " << dPrimaryTemperatureDelta;
        qWarning() << "ERROR! -> Calibration State: " << getCalibrationState();
    }
}

void CalibrationManager::fixControlProbeOffset()
{
    qDebug() << "CalibrationManager::fixControlProbeOffset()";

    m_tUpdateTemperatureValuesTimer.stop();

    double fContOffsetDelta;
    QString sDeviceType = m_pClient->getDeviceType();

    if ( sDeviceType.contains("Refrig", Qt::CaseInsensitive) )
    {
        // (REFERENCE READING [CHANNEL 1]) - (IC3 SETPOINT) = OFFSET
         fContOffsetDelta = m_dFlukeChannel1Temperature - TEMPERATURE_SETPOINT_4C_REFRIGERATOR;
    }
    else if ( sDeviceType.contains("Freezer", Qt::CaseInsensitive) )
    {
        // (REFERENCE READING [CHANNEL 1]) - (IC3 SETPOINT) = OFFSET
        fContOffsetDelta = m_dFlukeChannel1Temperature - TEMPERATURE_SETPOINT_NEG_30C_FREEZER;
    }
    else
    {
        qWarning() << "UNKNOWN DEVICE TYPE!!";
        return;
    }

    /* adjust control probe */
    //---------------------------------------------------------------------------------------------------
    // VALIDATE VALUE +/- 10.0
    if ( fContOffsetDelta <= OFFSET_VALUE_MIN ||
         fContOffsetDelta >= OFFSET_VALUE_MAX )
    {
        qWarning() << "[fContOffsetDelta = " << fContOffsetDelta << " -> ERROR!]";
        return;
    }
    double dRTD4_OffsetValue = m_dRTD4_OffsetValue + fContOffsetDelta;
    m_pClient->sendCalibrationRequest(eRTD4, dRTD4_OffsetValue);
    //---------------------------------------------------------------------------------------------------
    m_tUpdateTemperatureValuesTimer.start(ONE_MINUTE);
}

void CalibrationManager::fixPrimaryProbeOffset()
{
    qDebug() << "CalibrationManager::fixPrimaryProbeOffset()";

    m_tUpdateTemperatureValuesTimer.stop();
    /* match primary to reference */
    //---------------------------------------------------------------------------------------------------
    // (REFERENCE READING [CHANNEL 1]) - (IC3 READING [PRIMARY PROBE]) = OFFSET
    double fPrimOffsetDelta = m_dFlukeChannel1Temperature - m_dPrimaryTemperature;
    // VALIDATE VALUE +/- 10.0
    if ( fPrimOffsetDelta <= OFFSET_VALUE_MIN ||
         fPrimOffsetDelta >= OFFSET_VALUE_MAX )
    {
        qWarning() << "[fPrimOffsetDelta = " << fPrimOffsetDelta << " -> ERROR!]";
        return;
    }
    double dRTD5_OffsetValue = m_dRTD5_OffsetValue + fPrimOffsetDelta;
    m_pClient->sendCalibrationRequest(eRTD5, dRTD5_OffsetValue);
    //---------------------------------------------------------------------------------------------------
    m_tUpdateTemperatureValuesTimer.start(ONE_MINUTE);
}

void CalibrationManager::slot_UpdateTemperatureValuesTimeout()
{
//    qDebug() << "CalibrationManager::slot_OneMinuteTimeout()";

    m_dControlTemperature = m_pClient->getControlTemp();
    m_dFlukeChannel1Temperature = m_pClient->getFlukeTemp1();
    m_dRTD4_OffsetValue = m_pClient->getControlOffset();
    m_dRTD5_OffsetValue = m_pClient->getPrimaryOffset();

//    qDebug() << "m_dControlTemperature: " << m_dControlTemperature;
//    qDebug() << "m_dFlukeChannel1Temperature: " << m_dFlukeChannel1Temperature;
//    qDebug() << "m_dRTD4_OffsetValue: " << m_dRTD4_OffsetValue;
//    qDebug() << "m_dRTD5_OffsetValue: " << m_dRTD5_OffsetValue;
}

eCalibrationStates CalibrationManager::getCalibrationState()
{
    return m_eCalibrationState;
}

void CalibrationManager::setCalibrationState(eCalibrationStates eState)
{
    qDebug() << "CalibrationManager::setCalibrationState() -> " << eState;

    m_eCalibrationState = eState;

    if (m_eCalibrationState == eCALIBRATION_STATE_TEMPERATURE_STABLE)
    {
        m_bCompressorState = m_pClient->getCompressorState();

        //begin collecting compressor cycle information
        connect(&m_tCompressorStateCheckTimer, SIGNAL(timeout()), this, SLOT(slot_CompressorStateCheckTimeout()));
        m_tCompressorStateCheckTimer.start(ONE_SECOND);
    }
}

void CalibrationManager::slot_CompressorStateCheckTimeout()
{
    bool bNewCompressorState = m_pClient->getCompressorState();

    if ( m_bCompressorState == false &&
         bNewCompressorState == true )
    {
        qDebug() << "+ compressor is on +";


        compressorCycleStart();
        // enter cycle data to cycle array

        // check cycle array for calibrated critera
    }
    else if (m_bCompressorState == true &&
             bNewCompressorState == false )
    {
        qDebug() << "- compressor is off -";
    }

    m_bCompressorState = bNewCompressorState;
}

void CalibrationManager::compressorCycleStart()
{
    m_iNumberCompressorCycles++;

    if ( !(m_CycleDataList.isEmpty()) )
    {
        int i = m_CycleDataList.size()-1;
        m_CycleDataList[i].dtDateTimeEnd = QDateTime::currentDateTime();
        m_CycleDataList[i].dTempEnd = m_dFlukeChannel1Temperature;

        qDebug() << "========================================";
        qDebug() << "CycleNumber: " << m_CycleDataList[i].iCycleNumber;
        qDebug() << "DateTimeStart: " <<  m_CycleDataList[i].dtDateTimeStart;
        qDebug() << "TempStart: " <<  m_CycleDataList[i].dTempStart;
        qDebug() << "DateTimeEnd: " <<  m_CycleDataList[i].dtDateTimeEnd;
        qDebug() << "TempEnd: " <<  m_CycleDataList[i].dTempEnd;
        qDebug() << "========================================";
    }

    cycleData data;
    data.dtDateTimeStart = QDateTime::currentDateTime();
    data.dTempStart = m_dFlukeChannel1Temperature;
    data.iCycleNumber = m_iNumberCompressorCycles;

    m_CycleDataList.append(data);

    //if we have more than x # cycles in our m_CycleDataList, start checking if we are calibrated
    if ( m_CycleDataList.size() >= COMPRESSOR_CYCLES_CALIBRATION_CHECK)
    {
        checkIfCalibrated();
    }
    else
    {
        // not a large enough sample size yet to start checking for calibration
        qDebug() << "m_CycleDataList.size() -> " << m_CycleDataList.size();
    }

    // if we have x # cycles in our m_CycleDataList, and we are still not calibrated..
    if ( (m_CycleDataList.size() >= COMPRESSOR_CYCLES_ADJUSTMENT_CHECK) &&
         (getCalibrationState() != eCALIBRATION_STATE_CALIBRATED) )
    {
        checkIfAdjustmentsNeedMade();
    }
}

void CalibrationManager::checkIfCalibrated()
{
    qDebug() << "CalibrationManager::checkIfCalibrated()";

    double dSetpoint;
    QString sDeviceType = m_pClient->getDeviceType();

    if ( sDeviceType.contains("Refrig", Qt::CaseInsensitive) )
    {
         dSetpoint = TEMPERATURE_SETPOINT_4C_REFRIGERATOR;
    }
    else if ( sDeviceType.contains("Freezer", Qt::CaseInsensitive) )
    {
        dSetpoint = TEMPERATURE_SETPOINT_NEG_30C_FREEZER;
    }
    else
    {
        qWarning() << "UNKNOWN DEVICE TYPE!!";
        return;
    }

    // assume we are calibrated and prove we are not (if we are not)
    bool bCalibrated = true;

    int iIndexToStop = m_CycleDataList.size();
    iIndexToStop = iIndexToStop - COMPRESSOR_CYCLES_CALIBRATION_CHECK;

    // iterate through the last x number of cycles in the m_CycleDataList to check if we are calibrated or not
    for (int i = m_CycleDataList.size()-1; i >= iIndexToStop; i--)
    {
        if ( m_CycleDataList[i].dTempStart > (dSetpoint + CALIBRATED_TEMPERATURE_BUFFER) )
        {
            bCalibrated = false;
        }
        if ( m_CycleDataList[i].dTempStart < (dSetpoint - CALIBRATED_TEMPERATURE_BUFFER) )
        {
            bCalibrated = false;
        }
    }

    // change system state if we are calibrated
    if ( bCalibrated )
    {
        setCalibrationState(eCALIBRATION_STATE_CALIBRATED);
    }
}

void CalibrationManager::checkIfAdjustmentsNeedMade()
{
    qDebug() << "CalibrationManager::checkIfAdjustmentsNeedMade()";

    // if we are still not around our calibrated temperature goal, make further adjustments..
    double dSetpoint;
    QString sDeviceType = m_pClient->getDeviceType();

    if ( sDeviceType.contains("Refrig", Qt::CaseInsensitive) )
    {
         dSetpoint = TEMPERATURE_SETPOINT_4C_REFRIGERATOR;
    }
    else if ( sDeviceType.contains("Freezer", Qt::CaseInsensitive) )
    {
        dSetpoint = TEMPERATURE_SETPOINT_NEG_30C_FREEZER;
    }
    else
    {
        qWarning() << "UNKNOWN DEVICE TYPE!!";
        return;
    }

    double dPrimaryTemperature = m_pClient->getPrimaryTemp();

    if ( dPrimaryTemperature > (dSetpoint + CALIBRATED_TEMPERATURE_BUFFER) ||
         dPrimaryTemperature < (dSetpoint - CALIBRATED_TEMPERATURE_BUFFER) )
    {
        fixControlProbeOffset();
    }
}

// stable to unstable??? someone leaves door open for an extended period of time??


