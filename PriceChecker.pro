QT += quick widgets
QT += websockets network sql
QT += quickcontrols2
QT += core-private

SOURCES += \
        main.cpp \
#        src/arcirk.cpp \
#        src/httpservice.cpp \
#        src/serverresponse.cpp \
#        src/websocket.cpp \
    src/barcode_info.cpp \
    src/barcode_parser.cpp \
    src/httpservice.cpp \
    src/qjsontablemodel.cpp \
    src/qproxymodel.cpp \
    src/qtandroidservice.cpp \
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
    include/barcode_info.hpp \
    include/barcode_parser.hpp \
    include/database_struct.hpp \
    include/httpservice.hpp \
    include/includes.hpp \
    include/qjsontablemodel.h \
    include/qproxymodel.h \
    include/qtandroidservice.h \
    include/query_builder.hpp \
    include/shared_struct.hpp \
    include/websockets.hpp \
    include/wsSettings.hpp

INCLUDEPATH += $(BOOST_INCLDUE) #C:/lib/vcpkg/vcpkg/installed/x64-windows/include

#LIBS += -L$(BOOST_LIB) #-LC:/lib/vcpkg/vcpkg/installed/x64-windows/lib

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml\
    android/src/ru/arcirk/lscanner/qtandroidservice/ActivityUtils.java \
    android/src/ru/arcirk/lscanner/qtandroidservice/QtAndroidService.java

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
#    ANDROID_PACKAGE_SOURCE_DIR = \
#        $$PWD/android
    DEFINES += IS_OS_ANDROID
}
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


