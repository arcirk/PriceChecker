QT += quick
QT += websockets network sql
QT += quickcontrols2

SOURCES += \
        main.cpp \
#        src/arcirk.cpp \
#        src/httpservice.cpp \
#        src/serverresponse.cpp \
#        src/websocket.cpp \
    src/websockets.cpp \
        src/wsSettings.cpp

#resources.files = main.qml
#resources.prefix = /$${TARGET}
#RESOURCES += resources \
#    main.qrc
RESOURCES += main.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/includes.hpp \
    include/shared_struct.hpp \
    include/websockets.hpp \
    include/wsSettings.hpp

INCLUDEPATH += $(BOOST_INCLDUE) #C:/lib/vcpkg/vcpkg/installed/x64-windows/include

#LIBS += -L$(BOOST_LIB) #-LC:/lib/vcpkg/vcpkg/installed/x64-windows/lib
