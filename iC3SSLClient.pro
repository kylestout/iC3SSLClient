#-------------------------------------------------
#
# Project created by QtCreator 2011-07-11T20:39:20
#
#-------------------------------------------------

QT       += core gui network sql

TARGET = iC3SSLClient
TEMPLATE = app

INCLUDEPATH += ./qcustomplot \
               ./database \

SOURCES += main.cpp\
        client.cpp \
        QtJson.cpp \
        ./qcustomplot/qcustomplot.cpp \
        ./database/iC3_DatabaseTable.cpp \
        ./database/iC3_Database.cpp \
        ./database/iC3_DMM_UtilityFunctions.cpp \
        ./database/iC3_DatabaseColumnDef.cpp \
        ./database/iC3_TransducerTable.cpp \

HEADERS  += client.h \
            QtJson.h \
            ./qcustomplot/qcustomplot.h \
            ./database/iC3_DatabaseTable.h \
            ./database/iC3_Database.h \
            ./database/iC3_DMM_UtilityFunctions.h \
            ./database/iC3_DatabaseColumnDef.h \
            ./database/iC3_DMM_Constants.h \
            ./database/iC3_TransducerTable.h \


FORMS    += client.ui
