#ifndef DMM_CONSTANTS_H
#define DMM_CONSTANTS_H

enum eDateFormats
{
    eDATE_FORMAT_DDMMYY            = 0,
    eDATE_FORMAT_MMDDYY            = 1
};

enum eTimeFormats
{
    eTIME_FORMAT_NOT_SET           = -1,
    eTIME_FORMAT_24_HOUR           = 0,
    eTIME_FORMAT_12_HOUR           = 1
};

static const char HELMER_DB_CONNECTION_NAME[] = "HelmerDB_Connection";
static const char HELMER_DATABASE_FILE_NAME[] = "./database/LOG.db";

static const char CONFIG_FILE_PATH[] = "../data/config/app_config.xml";

static const char HELMER_DATA_FILE_PATH[] = "../data/";
static const char HELMER_TRANSDUCER_DATA_FILE_PATH[] = "../data/transducer_logs/";
static const char GRAPH_ERROR_LOG_PATH[] = "../data/Graph_Error_Log.txt";
static const char COMM_ERROR_LOG_PATH[] = "../data/DMM_Error_Log.txt";


static const char DEFAULT_EVENT_SEQUENCE_INDEX_FILE[] = "EventSequenceIndex-IC3.bin";

static const int TRANSDUCER_HEADER_NAME_LENGTH = 13;

// COMM_ERROR_REBOOT_FILE_PATH specifies a file that will be written if a comm error occurrs
// before the system shuts down for a reboot.  When the system reboots,  the
// iC3_DMM_Interface checks for the existence of this file.  If it exists, we know the reboot
// was caused by a comm error and we need to rename all old data files and start the system
// as if no data exists.  New database tables and data files will be created.
static const char COMM_ERROR_REBOOT_FILE_PATH[] = "../data/Comm_Error_Reboot.txt";

//-------------------------------------------------
// graph constants
//-------------------------------------------------
// if there is a comm error and we need to stop graph production,
// we will try for STOP_GRAPHS_WAIT_SECONDS to gracefully stop the graphs.
// If we can't stop the graphs within this time period, we will assume that
// the comm error is preventing it and continue to shut down.
static const int STOP_GRAPHS_WAIT_SECONDS   = 20;

static const int UPPER_ALARM_LIMIT_LINE_POS = 100;
static const int LOWER_ALARM_LIMIT_LINE_POS = 225;
static const int MAXIMUM_GRAPH_ALARMS_IN_ALARM_ICON = 100;

static const int NUMBER_OF_SECONDS_PER_DAY = 86400;  // 24 (hr) * 60 (min) * 60 (sec)

static const int ALARM_ICON_PIXEL_SEPARATION = 75;

static const int GRAPH_ALARM_LIMIT_LINE_PIXEL_WIDTH = 2;                     // 2 pixels
static const int SINGLE_VERTICAL_EPOCH_BORDER_LINE_WIDTH = 2;                // 2 pixels
static const int SPACE_BETWEEN_VERTICAL_LINES_IN_DOUBLE_LINE_BOUNDARY = 2;   // 2 pixels

static const int SINGLE_VERTICAL_EPOCH_BORDER_WIDTH = SINGLE_VERTICAL_EPOCH_BORDER_LINE_WIDTH;

static const int DOUBLE_VERTICAL_EPOCH_BORDER_WIDTH = SINGLE_VERTICAL_EPOCH_BORDER_LINE_WIDTH
                                                    + SPACE_BETWEEN_VERTICAL_LINES_IN_DOUBLE_LINE_BOUNDARY
                                                    + SINGLE_VERTICAL_EPOCH_BORDER_LINE_WIDTH;

enum eDMM_ErrorLevels
{
    eDMM_ERROR_LEVEL_CRITICAL                = -1,
    eDMM_ERROR_LEVEL_WARNING                 = 0,
    eDMM_ERROR_LEVEL_INFORMATION             = 1
};

enum eDMM_CommErrorLevels
{
    eDMM_COMM_ERROR_LEVEL_NONE                = 0,
    eDMM_COMM_ERROR_LEVEL_2                   = 2,
    eDMM_COMM_ERROR_LEVEL_3                   = 3
};

enum eGraphPixelTypes
{
    eEPOCH_PIXEL_WITH_DATA                   = 0,
    eEPOCH_BOUNDARY_SINGLE_LINE              = 1,
    eEPOCH_BOUNDARY_DOUBLE_LINE              = 2,
    eEPOCH_EMPTY_PIXEL                       = 5
};

enum eIC3_DatabaseRequestTypes
{
    eDB_REQUEST_NONE                         = -1,
    eDB_REQUEST_CREATE_TABLES                =  0,
    eDB_REQUEST_ADD_NEW_EVENT                =  1,
    eDB_REQUEST_GET_EVENT                    =  2,
    eDB_REQUEST_GET_LAST_N_EVENTS            =  3,
    eDB_REQUEST_GET_EVENTS_IN_ESIN_RANGE     =  4,
    eDB_REQUEST_MODIFY_EXISTING_EVENT        =  5,
    eDB_REQUEST_UPDATE_VIEWED_EVENTS         =  6,
    eDB_REQUEST_DOOR_OPENINGS_TODAY          =  7,
    eDB_REQUEST_DOOR_OPENINGS_DATE_RANGE     =  8,
    eDB_REQUEST_TOTAL_DOOR_OPENINGS          =  9,
    eDB_REQUEST_WRITE_CSV_FILES              =  10,
    eDB_REQUEST_INSERT_TRANS_SYNC_ENTRY      =  11,
    eDB_REQUEST_GET_TRANS_SYNC_DATA          =  12,
    eDB_REQUEST_INSERT_ACCESS_CONTROL_ENTRY  =  13,
    eDB_REQUEST_DELETE_ACCESS_CONTROL_ENTRY  =  14,
    eDB_REQUEST_GET_ACCESS_CONTROL_ENTRY     =  15,
    eDB_REQUEST_MODIFY_ACCESS_CONTROL_ENTRY  =  16,
    eDB_REQUEST_GET_ALL_ACCESS_CONTROL_DATA  =  17,
    eDB_REQUEST_INSERT_ACCESS_LOG_ENTRY      =  18,
    eDB_REQUEST_GET_ACCESS_LOG_ENTRY         =  19,
    eDB_REQUEST_UPDATE_ACCESS_LOG_ENTRY      =  20,
    eDB_REQUEST_UPDATE_VIEWED_ACCESS_LOG     =  21,
    eDB_REQUEST_GET_LAST_N_ACCESS_LOG_ENTRIES=  22,
    eDB_REQUEST_INSERT_AUTHORIZED_USER       =  23,
    eDB_REQUEST_GET_AUTHORIZED_USER          =  24,
    eDB_REQUEST_MODIFY_AUTHORIZED_USER       =  25,
    eDB_REQUEST_INSERT_USER_EVENT            =  26,
    eDB_REQUEST_GET_USER_EVENT               =  27,
    eDB_REQUEST_MODIFY_USER_EVENT            =  28,
    eDB_REQUEST_INSERT_DEFROST_EVENT         =  29,
    eDB_REQUEST_GET_DEFROST_EVENT            =  30,
    eDB_REQUEST_MODIFY_DEFROST_EVENT         =  31,
    eDB_REQUEST_GET_GRAPH_EPOCH_DATA         =  32,
    eDB_REQUEST_GET_GRAPH_DOOR_OPEN_DATA     =  33,
    eDB_REQUEST_GET_GRAPH_EVENT_DATA         =  34,
    eDB_REQUEST_GET_ESIN_RANGE_FOR_CSV_FILES =  35,
    eDB_REQUEST_PERFORM_INTEGRITY_CHECK      =  36
};

enum eIC3_TransducerRequestTypes
{
    eTRANSDUCER_REQUEST_NONE                 = -1,
    eTRANSDUCER_REQUEST_WRITE_LOG_HEADER     =  0,
    eTRANSDUCER_REQUEST_LOG_SAMPLE           =  1,
    eTRANSDUCER_REQUEST_GET_DATA_BY_ESIN     =  2,
    eTRANSDUCER_REQUEST_GET_DATA_BY_DATE     =  3,
    eTRANSDUCER_REQUEST_GET_GRAPH_DATA       =  4,
    eTRANSDUCER_REQUEST_WRITE_CSV_FILE       =  5
};

enum eIC3_TransducerType
{
    eTRANSDUCER_TYPE_NOT_SET = -1,
    eTEMPERATURE_TRANSDUCER  = 0,
    eVOLTAGE_TRANSDUCER      = 1,
    eHUMIDITY_TRANSDUCER     = 3,
    eCURRENT_TRANSDUCER      = 4,
    ePRESSURE_TRANSDUCER     = 5,
    eDIGITAL_TRANSDUCER      = 6,
    eLUMENS_TRANSDUCER       = 7,
    eDB_TRANSDUCER           = 8,
    eLOADCELL_TRANSDUCER     = 9,
    eACCELERATION_TRANSDUCER = 10,
    eTILT_TRANSDUCER         = 11,
    eFREQUENCY_TRANSDUCER    = 12,
    eRESISTANCE_TRANSDUCER    = 13
};

enum eIC3_GraphProductionStates
{
    eGRAPH_STATE_GRAPH_GENERATION_STOPPED         = -1,
    eGRAPH_STATE_IDLE                             = 0,
    eGRAPH_STATE_GET_EPOCH_DATA                   = 1,
    eGRAPH_STATE_GET_TRANSDUCER_DATA              = 2,
    eGRAPH_STATE_GENERATE_PIXEL_FRAME_ARRAY       = 3,
    eGRAPH_STATE_GET_DOOR_OPENING_DATA            = 4,
    eGRAPH_STATE_GENERATE_GRAPH_ALARM_FRAME_ARRAY = 5,
    eGRAPH_STATE_DRAWING_GRAPH                    = 6,
    eGRAPH_STATE_DRAWING_GRAPH_LABELS             = 7,
    eGRAPH_STATE_GRAPH_TRANSDUCER_ERROR           = 8,
    eGRAPH_STATE_GRAPH_DATA_ERROR                 = 9,
    eGRAPH_STATE_GRAPH_DB_ERROR                   = 10
};

enum CSV_FileGeneratorStates
{
    eCSV_GENERATOR_UNINITIALIZED              = -1,
    eCSV_GENERATOR_IDLE                       = 0,
    eCSV_GENERATOR_WAITING_FOR_ESIN_RANGE     = 1,
    eCSV_GENERATOR_WRITING_DB_FILES           = 2,
    eCSV_GENERATOR_WRITING_TRANSDUCER_FILES   = 3
};

#define CHAMBER_TRANSDUCER_ID 107
#define AMBIENT_TRANSDUCER_ID 111
#define HS_DISCHARGE_TRANSDUCER_ID 257
#define HEAT_EXCHANGER_TRANSDUCER_ID 109
#define HS_CONDENSER_TRANSDUCER_ID 110
#define LS_DISCHARGE_TRANSDUCER_ID 256

#define VIRTUAL_PROBE_TRANSDUCER_ID 267
#define MAINS_VOLTAGE_TRANSDUCER_ID 200

// define which of the transducer is the primary transducer and which is the secondary transducer.
// This is used while graphing transducer data to determine whether the alarm limits for a transducer
// should be pulled from the transducer synchronization table primary alarm limits columns or the
// transducer synchronization table secondary alarm limits columns
#define PRIMARY_TRANSDUCER_ID    VIRTUAL_PROBE_TRANSDUCER_ID
#define SECONDARY_TRANSDUCER_ID  AMBIENT_TRANSDUCER_ID

#endif // DMM_CONSTANTS_H
