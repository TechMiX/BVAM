#-------------------------------------------------
#
# Project created by QtCreator 2012-11-16T01:25:01
#
#-------------------------------------------------

QT       += core gui

TARGET = VanityAddressManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
	BitcoinCrypto.cpp \
    newprefixdialog.cpp \
    solvedialog.cpp

HEADERS  += mainwindow.h \
	 BitcoinCrypto.h \
    newprefixdialog.h \
    solvedialog.h

FORMS    += mainwindow.ui \
    newprefixdialog.ui \
    solvedialog.ui

windows {
    CONFIG += static
    INCLUDEPATH += $$quote(C:\OpenSSL-Win32\include)
    LIBS += -L$$quote(C:\OpenSSL-Win32\lib) -lssleay32 -llibeay32
    RC_FILE = bvam.rc
}

!windows:LIBS += -lssl -lcrypto

OTHER_FILES +=

RESOURCES += \
    res.qrc
