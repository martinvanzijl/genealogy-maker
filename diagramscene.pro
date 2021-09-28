QT += printsupport widgets xml

HEADERS	    =   mainwindow.h \
		diagramitem.h \
		diagramscene.h \
		arrow.h \
		diagramtextitem.h \
		mygraphicsview.h \
		percentvalidator.h \
    undo/additemundo.h \
    undo/deleteitemsundo.h \
    undo/addarrowundo.h \
    undo/moveitemsundo.h
SOURCES	    =   mainwindow.cpp \
		diagramitem.cpp \
		main.cpp \
		arrow.cpp \
		diagramtextitem.cpp \
		diagramscene.cpp \
		mygraphicsview.cpp \
		percentvalidator.cpp \
    undo/additemundo.cpp \
    undo/deleteitemsundo.cpp \
    undo/addarrowundo.cpp \
    undo/moveitemsundo.cpp
RESOURCES   =	diagramscene.qrc


# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/diagramscene
INSTALLS += target
