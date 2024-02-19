QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    messagecontroller.cpp \
    socketcontroller.cpp

HEADERS += \
    mainwindow.h \
    messagecontroller.h \
    socketcontroller.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

!android:{
    win32:QMAKE_POST_LINK += windeployqt --force --widgets --network --qml --quick --core --gui $$shell_quote($$DESTDIR)
}
