TARGET   = SeisViewer
CONFIG  += plugin
QT += xml


#CARNAC_DIR_VAR = $$(CARNAC_DIR)
    QMAKE_LIBDIR += $$(SPRINTROOT)/lib $$(CARNAC_DIR)/lib $$(CWPROOT)/lib


    LIBS += -lCarnacSeismicQt4 -lCarnacSeismic  \
            -lCarnacPlotQt4 -lCarnacDriverQt4  \
            -lCarnacPluginQt4 -lCarnacQt4 -lCarnacCGM  \
            -lCarnacPlot -lCarnacX -lCarnacPS -lCarnac  \
            -lXt -lQtGui -lpng -lSM -lICE -lXi -lXrender  \
            -lXrandr -lXfixes -lXcursor -lXinerama -lfreetype  \
            -lfontconfig -lXext -lX11 -lQtCore  \
            -lSPFramework \
            -lsu -lpar -lcwp -lsqlite3 \
            -lz -lm -lrt -ldl -lpthread


HEADERS += \
    src/spaxis.h \
    src/spcolormaps.h \
    src/spdetail.h \
    src/spframedial.h \
    src/sphelp.h \
    src/spkeys.h \
    src/spprinter.h \
    src/spprojector.h \
    src/spsection.h \
    src/spspeeddial.h \
    src/spviewer.h

SOURCES += \
    src/main.cpp \
    src/spaxis.cpp \
    src/spcolormaps.cpp \
    src/spdetail.cpp \
    src/spkeys.cpp \
    src/spprinter.cpp \
    src/spprojector.cpp \
    src/spsection.cpp \
    src/spviewer.cpp

FORMS += \
    src/spviewer.ui \
    src/spprinter.ui \
    src/spdials.ui \
    src/spdetail.ui \
    src/spaxis.ui

INCLUDEPATH += $$(CARNAC_DIR)/include $$(CARNAC_DIR)/include/QtPlot/QtGui \
            $$(CARNAC_DIR)/include/QtGui \
            $$(SPRINTROOT)/include $$(CWPROOT)/include \
            /usr/lib64/qt4/mkspecs/linux-g++-64 \
            /usr/include/QtCore \
            /usr/include/QtGui \
            /usr/include

RESOURCES += \
    src/icons.qrc
