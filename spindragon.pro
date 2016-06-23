TEMPLATE = app
TARGET = spindragon
INCLUDEPATH += .

MOC_DIR = .moc
OBJECTS_DIR = .obj
RCC_DIR = .rcc
UI_DIR = .ui

HEADERS += \
    spindragon.h \
    errors.h \
    tokens.h \
    paths.h \

SOURCES += \
    main.cpp \
    spindragon.cpp \
    errors.cpp \
    paths.cpp \

