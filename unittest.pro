QT += help printsupport widgets xml
QT += testlib

HEADERS	    =   \
		diagramitem.h \
		diagramscene.h \
		arrow.h \
		diagramtextitem.h \
		mygraphicsview.h \
		percentvalidator.h \
    undo/additemundo.h \
    undo/deleteitemsundo.h \
    undo/addarrowundo.h \
    undo/moveitemsundo.h \
    gui/dialogfind.h \
    gui/dialogpersondetails.h \
    marriageitem.h \
    undo/marriageundo.h \
    gui/dialogmarriagedetails.h \
    undo/removemarriageundo.h \
    undo/undomanager.h \
    gui/mainform.h \
    gui/dialoghelp.h \
    gui/dialogchangesize.h \
    gui/dialogviewphoto.h \
    undo/changefillcolorundo.h \
    draggablebutton.h \
    gui/preferenceswindow.h
SOURCES	    =   \
		diagramitem.cpp \
		testcases.cpp \
		arrow.cpp \
		diagramtextitem.cpp \
		diagramscene.cpp \
		mygraphicsview.cpp \
		percentvalidator.cpp \
    undo/additemundo.cpp \
    undo/deleteitemsundo.cpp \
    undo/addarrowundo.cpp \
    undo/moveitemsundo.cpp \
    gui/dialogfind.cpp \
    gui/dialogpersondetails.cpp \
    marriageitem.cpp \
    undo/marriageundo.cpp \
    gui/dialogmarriagedetails.cpp \
    undo/removemarriageundo.cpp \
    undo/undomanager.cpp \
    gui/mainform.cpp \
    gui/dialoghelp.cpp \
    gui/dialogchangesize.cpp \
    gui/dialogviewphoto.cpp \
    undo/changefillcolorundo.cpp \
    draggablebutton.cpp \
    gui/preferenceswindow.cpp
RESOURCES   =	genealogymaker.qrc

FORMS += \
    gui/dialogfind.ui \
    gui/dialogpersondetails.ui \
    gui/dialogmarriagedetails.ui \
    gui/mainform.ui \
    gui/dialoghelp.ui \
    gui/dialogchangesize.ui \
    gui/dialogviewphoto.ui \
    gui/preferenceswindow.ui
