cmake_minimum_required(VERSION 3.17)

SET(CMAKE_PREFIX_PATH "/usr/lib64/qt4")

project(spViewer)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


#find_package(Qt5 COMPONENTS Widgets DBus REQUIRED)

#target_link_libraries(spViewer Qt5::Widgets Qt5::DBus)

set(Qt_SELECTOR "Qt4")

if (Qt_SELECTOR MATCHES "Qt4") 

    message("Looking for Qt4...")

    find_package(Qt4 REQUIRED)
    if (${Qt4_FOUND})
        message("Found Qt " ${Qt4_VERSION})
    else()
        message("Couldn't find Qt4")
    endif()

else ()

    message("Looking for Qt5...")

    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    if (${Qt5_FOUND})
        message("Found Qt " ${Qt5_VERSION})
    else()
        message("Couldn't find Qt5")
    endif()

    get_target_property(QtCore_location Qt5::Widgets LOCATION)
    message("Qt5 location: ${QtCore_location}")

endif()

# your source files
set(sources
    main.cpp
    spaxis.ui
    spdials.ui
    spdetail.ui
    spprinter.ui
    spviewer.ui
    spaxis.h
    spaxis.cpp
    spcolormaps.h
    spcolormaps.cpp
    spdetail.h
    spdetail.cpp
    spframedial.h
    sphelp.h
    spkeys.h
    spkeys.cpp
    spprinter.h
    spprinter.cpp
    spprojector.h
    spprojector.cpp
    spsection.h
    spsection.cpp
    spspeeddial.h
    spviewer.h
    spviewer.cpp
    icons.qrc
)

# name of the .exe file, window flag and the list of things to compile
add_executable(${CMAKE_PROJECT_NAME}  ${sources})

#target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Widgets)

# dependancies to Qt4
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC /usr/include/QtCore  /usr/include/QtGui)
#target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{CARNAC_DIR}/lib)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC QtCore QtGui QtXml)

# dependancies to CARNAC
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{CARNAC_DIR}/include)
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{CARNAC_DIR}/lib)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC Carnac CarnacPlotQt4 CarnacSeismic CarnacSeismicQt4 CarnacQt4 CarnacCGM CarnacPlot)

# dependancies to spDB
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{SPRINTROOT}/include)
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{SPRINTROOT}/lib)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC SPFramework)

# dependancies to SeismicUnix
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{CWPROOT}/include)
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC $ENV{CWPROOT}/lib)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC su par cwp)

