cmake_minimum_required(VERSION 3.16)

project(Client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network WebSockets)

set(PROJECT_SOURCES
        main.cpp
        mainwidget.cpp
        mainwidget.h
        mainwidget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Client
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        registerwidget.h registerwidget.cpp
        CoreData.h
        datacenter.h datacenter.cpp
        datacenter.h
        datacenter.cpp

        DataCenterCoreData.h
        loginbynamewidget.h
        loginbynamewidget.cpp
        loginbyphone.h
        loginbyphone.cpp
        consumerdishlistwidget.h
        consumerdishlistwidget.cpp
        Nav.h
        consumercartlistwidget.h
        consumercartlistwidget.cpp
        consumerorderlistwidget.h
        consumerorderlistwidget.cpp
        consumerorderdetailwidget.h
        consumerorderdetailwidget.cpp

        consumerorderdetailwidget.cpp
        consumerdishdetailwidget.cpp
        consumerdishdetailwidget.h
        consumerdishdetailwidget.h
        consumerdishdetailwindow.h
        merchantdishlistwidget.h
        merchantdishlistwidget.cpp
        merchantdishlistwidget.cpp
        merchantorderlistwidget.h
        merchantorderlistwidget.cpp
        merchantdishegisterwidget.h merchantdishregisterwidget.cpp merchantdishregisterwindow.h
        merchantorderdetailwindow.h merchantorderdetailwidget.cpp merchantorderdetailwidget.h
        merchantdishdetailwidget.cpp merchantdishdetailwidget.h merchantdishdetailwindow.h
        merchantdisheditwidget.cpp merchantdisheditwidget.h merchantdisheditwindow.h
        adminorderlistwidget.cpp adminorderlistwidget.h
        thirdpaywindow.h
        consumervipwidget.cpp consumervipwidget.h
        consumeruserinfowidget.cpp consumeruserinfowidget.h
        merchantuserinfowidget.cpp merchantuserinfowidget.h

        ServerConfig.json
        NetClient.h NetClient.cpp
        qimg.qrc




    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Client APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Client SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Client
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Client PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Network Qt6::WebSockets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Client)
endif()
set_target_properties(Client PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    AUTOMOC TRUE
    AUTOUIC TRUE
    AUTORCC TRUE
)

include(GNUInstallDirs)
install(TARGETS Client
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Client)
endif()
