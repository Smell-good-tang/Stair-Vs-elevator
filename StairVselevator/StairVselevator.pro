QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    efloors.cpp

HEADERS += \
    efloors.h

FORMS += \
    efloors.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

CONFIG(debug, debug|release)
   {
        DEFINES += VLD_MODULE
        VLD_PATH = "C:/Program Files (x86)/Visual Leak Detector"
        INCLUDEPATH += $${VLD_PATH}/include
        LIBS += -L$${VLD_PATH}/lib/Win64 -lvld
   }
