#-------------------------------------------------
#
# Project created by QtCreator 2011-09-14T11:37:37
#
#-------------------------------------------------

QT   += core gui svg

TARGET = SecondGUI
TEMPLATE = app
CONFIG += console #for better display of processes with std::cout.flush()
QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -fopenmp
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-result
LIBS += -fopenmp

#DSP
INCLUDEPATH += ./DSP
LIBS += -L../SecondGUI/DSP -ldspFilters

#new arrangement
#INCLUDEPATH += ./classifier
#INCLUDEPATH += ./other
#INCLUDEPATH += ./widgets
#INCLUDEPATH += ./myLib
#INCLUDEPATH += ./other

#cwt
INCLUDEPATH += ./matlab/for_testing
DEPENDPATH += ./matlab/for_testing
INCLUDEPATH += ./matlab/mcr_extern_include
LIBS += -L../SecondGUI/matlab/for_testing -lcwt_r

LIBS += -L/media/Files/Matlab/MCR/v83/bin/glnxa64 -leng -lmwcpp11compat
LIBS += -L/media/Files/Matlab/MCR/v83/runtime/glnxa64 -lmwmclmcrrt

SOURCES += main.cpp\
widgets/mainwindow/mainwindow.cpp \
widgets/mainwindow/mainwindow_slices.cpp \
widgets/mainwindow/mainwindow_ica.cpp \
widgets/mainwindow/mainwindow_editedf.cpp \
widgets/mainwindow/mainwindow_autos.cpp \
widgets/mainwindow/mainwindow_customF.cpp \
myLib/myLib_clustering.cpp \
myLib/myLib_signalProcessing.cpp \
myLib/myLib_draws.cpp \
myLib/myLib_dataHandler.cpp \
myLib/myLib_drw.cpp \
myLib/myLib_wavelet.cpp \
myLib/myLib_statistics.cpp \
myLib/myLib_output.cpp \
myLib/myLib_mati.cpp \
myLib/myLib_iitp.cpp \
myLib/myLib_valar.cpp \
myLib/myLib_general.cpp \
classifier/classifier-data.cpp \
classifier/classifier_lib.cpp \
classifier/classifier.cpp \
classifier/classifier_ann.cpp \
classifier/classifier_svm.cpp \
classifier/classifier_dist.cpp \
classifier/classifier_nbc.cpp \
classifier/classifier_knn.cpp \
classifier/classifier_word.cpp \
classifier/classifier_rda.cpp \
other/autos.cpp \
other/edffile.cpp \
other/edfFile_repair.cpp \
other/coord.cpp \
other/matrix.cpp \
widgets/net/net.cpp \
widgets/net/net_classification.cpp \
widgets/net/net_setsgets.cpp \
widgets/net/net_successive.cpp \
widgets/net/net_data.cpp \
widgets/spectre.cpp \
widgets/cut.cpp

HEADERS  += \
widgets/cut.h \
myLib/drw.h \
myLib/signalProcessing.h \
myLib/wavelet.h \
myLib/dataHandlers.h \
myLib/draws.h \
myLib/statistics.h \
myLib/clustering.h \
myLib/output.h \
myLib/mati.h \
myLib/iitp.h \
myLib/valar.h \
myLib/small.h \
myLib/general.h \
DSP/DspFilters/Dsp.h \
DSP/DspFilters/Bessel.h \
DSP/DspFilters/Biquad.h \
DSP/DspFilters/Butterworth.h \
DSP/DspFilters/Cascade.h \
DSP/DspFilters/ChebyshevI.h \
DSP/DspFilters/ChebyshevII.h \
DSP/DspFilters/Common.h \
DSP/DspFilters/Custom.h \
DSP/DspFilters/Design.h \
DSP/DspFilters/Elliptic.h \
DSP/DspFilters/Filter.h \
DSP/DspFilters/Layout.h \
DSP/DspFilters/Legendre.h \
DSP/DspFilters/MathSupplement.h \
DSP/DspFilters/Params.h \
DSP/DspFilters/PoleFilter.h \
DSP/DspFilters/RBJ.h \
DSP/DspFilters/RootFinder.h \
DSP/DspFilters/SmoothedFilter.h \
DSP/DspFilters/State.h \
DSP/DspFilters/Types.h \
DSP/DspFilters/Utilities.h \
widgets/mainwindow.h \
widgets/net.h \
widgets/spectre.h \
classifier/classifier.h \
classifier/classifierdata.h \
other/autos.h \
other/coord.h \
other/matrix.h \
other/edffile.h

FORMS+= mainwindow.ui \
cut.ui \
net.ui \
spectre.ui

OTHER_FILES +=

RESOURCES +=
