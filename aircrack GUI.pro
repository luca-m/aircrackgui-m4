#-------------------------------------------------
#
# Project created by QtCreator 2010-12-03T00:39:21
#
#-------------------------------------------------

QT       += core gui

TARGET = aircrack-GUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    infoessid.cpp \
    utils.cpp \
    infoconnectionbssid.cpp \
    airodump.cpp \
    airmon.cpp \
    setinterface.cpp \
    GLOBALS.cpp \
    aireplaydeauth.cpp \
    aireplayauth.cpp \
    aireplaybroadcast.cpp \
    macchanger.cpp \
    aircrack.cpp \
    options.cpp \
    aireplayarpreplay.cpp \
    historicalnetwork.cpp \
    updater.cpp \
    fragmentation.cpp \
    chopchop.cpp \
    debug.cpp

HEADERS  += mainwindow.h \
    infoessid.h \
    utils.h \
    DEFINES.h \
    infoconnectionbssid.h \
    airodump.h \
    airmon.h \
    setinterface.h \
    GLOBALS.h \
    aireplaydeauth.h \
    aireplayauth.h \
    aireplaybroadcast.h \
    macchanger.h \
    aircrack.h \
    options.h \
    aireplayarpreplay.h \
    historicalnetwork.h \
    updater.h \
    fragmentation.h \
    chopchop.h \
    debug.h

FORMS    += UI/mainwindow.ui \
    UI/airodump.ui \
    UI/airmon.ui \
    UI/setinterface.ui \
    UI/aireplaydeauth.ui \
    UI/aireplayauth.ui \
    UI/aireplaybroadcast.ui \
    UI/macchanger.ui \
    UI/aircrack.ui \
    UI/options.ui \
    UI/airodump.ui \
    UI/airmon.ui \
    UI/setinterface.ui \
    UI/aireplayauth.ui \
    UI/aireplaydeauth.ui \
    UI/aireplaybroadcast.ui \
    UI/macchanger.ui \
    UI/aircrack.ui \
    UI/options.ui \
    UI/aireplayarpreplay.ui \
    UI/historicalnetwork.ui \
    UI/updater.ui \
    UI/fragmentation.ui \
    UI/chopchop.ui

RESOURCES += \
    icons.qrc

OTHER_FILES += \
    changelog.txt \
    aircrack-GUI-Start.sh
