TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

CONFIG += warn_off
DEFINES += LINUX386

SOURCES += main.cpp \
    mat.cpp \
    BCA.cpp \
    util.cpp \
    mat2.cpp

LIBS += -lGL -lGLU -lglut -lgsl -lgslcblas /home/jaewooklee/irit/circlink/circlink.o -L/home/jaewooklee/irit/lib -lIritExt -lIritGrap -lIritUser -lIritRndr -lIritBool -lIritPrsr -lIritMdl -lIritMvar -lIritTrim -lIritTriv -lIritTrng -lIritSymb -lIritCagd -lIritGeom -lIritMisc -lIritXtra


HEADERS += \
    Point.h \
    Bezier.h \
    Poly.h \
    Curve.h \
    util.h \
    Geom.h \
    BCA.h \
    mat.h \
    mat2.h


QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

OTHER_FILES +=

INCLUDEPATH += /home/jaewooklee/irit/inc_irit
