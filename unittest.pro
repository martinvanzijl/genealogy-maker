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
    gui/preferenceswindow.h \
    undo/editpersondetailsundo.h \
    fileutils.h \
    undo/changetextcolorundo.h \
    undo/changelinecolorundo.h \
    viewphotowindow.h \
    gui/reportwindow.h \
    gui/timelinereportwindow.h \
    gui/dialogfileproperties.h \
    undo/undoblock.h \
    undo/changebordercolorundo.h \
    undo/changediagramsizeundo.h
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
    gui/preferenceswindow.cpp \
    undo/editpersondetailsundo.cpp \
    fileutils.cpp \
    undo/changetextcolorundo.cpp \
    undo/changelinecolorundo.cpp \
    viewphotowindow.cpp \
    gui/reportwindow.cpp \
    gui/timelinereportwindow.cpp \
    gui/dialogfileproperties.cpp \
    undo/undoblock.cpp \
    undo/changebordercolorundo.cpp \
    undo/changediagramsizeundo.cpp
RESOURCES   =	genealogymaker.qrc

FORMS += \
    gui/dialogfind.ui \
    gui/dialogpersondetails.ui \
    gui/dialogmarriagedetails.ui \
    gui/mainform.ui \
    gui/dialoghelp.ui \
    gui/dialogchangesize.ui \
    gui/dialogviewphoto.ui \
    gui/preferenceswindow.ui \
    gui/reportwindow.ui \
    gui/timelinereportwindow.ui \
    gui/dialogfileproperties.ui

# Copy example files. Does not work!
win32 {
    # Windows.
} else {
    # Linux.
    example_files.target = $$OUT_PWD/examples
    example_files.commands = cp -R $$PWD/examples $$OUT_PWD

    QMAKE_EXTRA_TARGETS += example_files

    message("Extra targets: $$QMAKE_EXTRA_TARGETS")
    message("Target: $$OUT_PWD/examples")
    message("Command: cp -R $$PWD/examples $$OUT_PWD")

    PRE_TARGETDEPS += $$OUT_PWD/examples
}
