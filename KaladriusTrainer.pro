QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg xml quick quickwidgets sql charts

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "Sources"

win32 {
    # WARNING: boost version is 1_73_0
    # I built boost_serialization with the following command lines:
    #       .\bootstrap.bat
    #       .\b2 address-model=64 --with-serialization
    INCLUDEPATH += "../boost_1_73_0"
    LIBS += "-L../boost_1_73_0/stage/lib"
}

unix {
    # WARNING: boost version is 1_73_0
    # I built boost_serialization with the following command lines:
    #       ./bootstrap.sh --prefix=/home/fromtonrouge/.local --with-libraries=serialization
    #       ./b2 install
    INCLUDEPATH += "/home/fromtonrouge/.local/include"
    LIBS += "-L/home/fromtonrouge/.local/lib" "-lboost_serialization"
}

RC_ICONS = Resources/KaladriusTrainer.ico

SOURCES += \
    Sources/Charts/ChartView.cpp \
    Sources/Dashboard/Dashboard.cpp \
    Sources/Database/Database.cpp \
    Sources/Dictionaries/DictionariesModel.cpp \
    Sources/Dictionaries/DictionariesWidget.cpp \
    Sources/Dictionaries/Dictionary.cpp \
    Sources/Dictionaries/DictionaryParser.cpp \
    Sources/Dictionaries/Keycode.cpp \
    Sources/Keyboards/KeyboardEditorMainWindow.cpp \
    Sources/Keyboards/KeyboardGraphicsScene.cpp \
    Sources/Keyboards/KeyboardGraphicsView.cpp \
    Sources/Keyboards/KeyboardTreeView.cpp \
    Sources/Keyboards/Models/KeyboardModel.cpp \
    Sources/Keyboards/Models/UndoableKeyboardModel.cpp \
    Sources/Keyboards/TreeItems/KeyboardTreeItem.cpp \
    Sources/Keyboards/TreeItems/KeycapTreeItem.cpp \
    Sources/Keycaps/KeycapColorEffect.cpp \
    Sources/Keycaps/KeycapDelegate.cpp \
    Sources/Keycaps/KeycapGraphicsItem.cpp \
    Sources/Keycaps/KeycapTreeView.cpp \
    Sources/Keycaps/KeycapWidget.cpp \
    Sources/Levels/LevelsTreeView.cpp \
    Sources/Levels/Models/LevelsModel.cpp \
    Sources/Levels/TreeItems/LevelTreeItem.cpp \
    Sources/Levels/TreeItems/LevelsTreeItem.cpp \
    Sources/Main/Application.cpp \
    Sources/Main/MainTabDialog.cpp \
    Sources/Main/MainTabWindow.cpp \
    Sources/Main/MainWindow.cpp \
    Sources/Main/main.cpp \
    Sources/Serialization/Finger.cpp \
    Sources/Serialization/KeycapRef.cpp \
    Sources/Serialization/ListValue.cpp \
    Sources/Serialization/Qt/QByteArray.cpp \
    Sources/Serialization/Qt/QStandardItem.cpp \
    Sources/Serialization/Qt/QString.cpp \
    Sources/Serialization/Qt/QVariant.cpp \
    Sources/Serialization/Serialize.cpp \
    Sources/Statistics/StatisticsMainWindow.cpp \
    Sources/Statistics/StatisticsView.cpp \
    Sources/Statistics/StatisticsWidget.cpp \
    Sources/StrokesSolver/StrokesSolverTextEdit.cpp \
    Sources/StrokesSolver/StrokesSolverWidget.cpp \
    Sources/StrokesSolver/TypingTestResult.cpp \
    Sources/StrokesSolver/WordCounter.cpp \
    Sources/StrokesSolver/WordsToImprove.cpp \
    Sources/Theories/Models/TheoryModel.cpp \
    Sources/Theories/Models/UndoableTheoryModel.cpp \
    Sources/Theories/TheoryEditorMainWindow.cpp \
    Sources/Theories/TheoryTreeView.cpp \
    Sources/Theories/TreeItems/DictionaryTreeItem.cpp \
    Sources/Theories/TreeItems/InputKeysTreeItem.cpp \
    Sources/Theories/TreeItems/LinkedDictionaryTreeItem.cpp \
    Sources/Theories/TreeItems/LinkedTheoryTreeItem.cpp \
    Sources/Theories/TreeItems/OutputTextTreeItem.cpp \
    Sources/Theories/TreeItems/TheoryTreeItem.cpp \
    Sources/Tree/Models/DiffModel.cpp \
    Sources/Tree/Models/InsertBranchCommand.cpp \
    Sources/Tree/Models/RemoveBranchCommand.cpp \
    Sources/Tree/Models/SetDataCommand.cpp \
    Sources/Tree/Models/TreeItemModel.cpp \
    Sources/Tree/Models/UndoableProxyModel.cpp \
    Sources/Tree/Models/Utils.cpp \
    Sources/Tree/TreeItems/AttributeTreeItem.cpp \
    Sources/Tree/TreeItems/ListTreeItem.cpp \
    Sources/Tree/TreeItems/TreeItem.cpp \
    Sources/Tree/TreeItems/ValueTreeItem.cpp \
    Sources/Utils/AverageOf.cpp \
    Sources/Utils/CountdownTimer.cpp \
    Sources/Utils/Functions.cpp \
    Sources/Values/Editors/ColorEditor.cpp \
    Sources/Values/Editors/FingerSelector.cpp \
    Sources/Values/Editors/FontEditor.cpp \
    Sources/Values/Editors/KeycapRefEditor.cpp \
    Sources/Values/Editors/UserEditor.cpp \
    Sources/Values/Editors/UserEditorFactory.cpp \
    Sources/Values/Editors/UserItemDelegate.cpp

HEADERS += \
    Resources/KaladriusTrainer.rc \
    Resources/resource.h \
    Sources/Charts/ChartView.h \
    Sources/Dashboard/Dashboard.h \
    Sources/Database/Database.h \
    Sources/Dictionaries/DictionariesModel.h \
    Sources/Dictionaries/DictionariesWidget.h \
    Sources/Dictionaries/Dictionary.h \
    Sources/Dictionaries/DictionaryParser.h \
    Sources/Dictionaries/Keycode.h \
    Sources/Keyboards/KeyboardEditorMainWindow.h \
    Sources/Keyboards/KeyboardGraphicsScene.h \
    Sources/Keyboards/KeyboardGraphicsView.h \
    Sources/Keyboards/KeyboardTreeView.h \
    Sources/Keyboards/Models/KeyboardModel.h \
    Sources/Keyboards/Models/UndoableKeyboardModel.h \
    Sources/Keyboards/TreeItems/KeyboardTreeItem.h \
    Sources/Keyboards/TreeItems/KeycapTreeItem.h \
    Sources/Keycaps/KeycapColorEffect.h \
    Sources/Keycaps/KeycapDelegate.h \
    Sources/Keycaps/KeycapGraphicsItem.h \
    Sources/Keycaps/KeycapState.h \
    Sources/Keycaps/KeycapTreeView.h \
    Sources/Keycaps/KeycapWidget.h \
    Sources/Levels/LevelsTreeView.h \
    Sources/Levels/Models/LevelsModel.h \
    Sources/Levels/TreeItems/LevelTreeItem.h \
    Sources/Levels/TreeItems/LevelsTreeItem.h \
    Sources/License.h \
    Sources/Main/Application.h \
    Sources/Main/MainTabDialog.h \
    Sources/Main/MainTabWindow.h \
    Sources/Main/MainWindow.h \
    Sources/Serialization/Finger.h \
    Sources/Serialization/KeycapRef.h \
    Sources/Serialization/ListValue.h \
    Sources/Serialization/ExplicitInstanciation.h \
    Sources/Serialization/Qt/QByteArray.h \
    Sources/Serialization/Qt/QStandardItem.h \
    Sources/Serialization/Qt/QString.h \
    Sources/Serialization/Qt/QVariant.h \
    Sources/Serialization/Qt/QVector.h \
    Sources/Serialization/Serialize.h \
    Sources/Statistics/StatisticsMainWindow.h \
    Sources/Statistics/StatisticsView.h \
    Sources/Statistics/StatisticsWidget.h \
    Sources/Streams/Iostream.h \
    Sources/Streams/StreamSink.h \
    Sources/StrokesSolver/StrokesSolverTextEdit.h \
    Sources/StrokesSolver/StrokesSolverWidget.h \
    Sources/StrokesSolver/TypingTestResult.h \
    Sources/StrokesSolver/WordCounter.h \
    Sources/StrokesSolver/WordsToImprove.h \
    Sources/Theories/Models/TheoryModel.h \
    Sources/Theories/Models/UndoableTheoryModel.h \
    Sources/Theories/TheoryEditorMainWindow.h \
    Sources/Theories/TheoryTreeView.h \
    Sources/Theories/TreeItems/DictionaryTreeItem.h \
    Sources/Theories/TreeItems/InputKeysTreeItem.h \
    Sources/Theories/TreeItems/LinkedDictionaryTreeItem.h \
    Sources/Theories/TreeItems/LinkedTheoryTreeItem.h \
    Sources/Theories/TreeItems/OutputTextTreeItem.h \
    Sources/Theories/TreeItems/TheoryTreeItem.h \
    Sources/Tree/Models/DiffModel.h \
    Sources/Tree/Models/InsertBranchCommand.h \
    Sources/Tree/Models/ItemDataRole.h \
    Sources/Tree/Models/RemoveBranchCommand.h \
    Sources/Tree/Models/SetDataCommand.h \
    Sources/Tree/Models/TreeItemModel.h \
    Sources/Tree/Models/UndoableProxyModel.h \
    Sources/Tree/Models/Utils.h \
    Sources/Tree/TreeItems/AttributeTreeItem.h \
    Sources/Tree/TreeItems/ListTreeItem.h \
    Sources/Tree/TreeItems/TreeItem.h \
    Sources/Tree/TreeItems/ValueTreeItem.h \
    Sources/Utils/AverageOf.h \
    Sources/Utils/CountdownTimer.h \
    Sources/Utils/Functions.h \
    Sources/Values/Editors/ColorEditor.h \
    Sources/Values/Editors/FingerSelector.h \
    Sources/Values/Editors/FontEditor.h \
    Sources/Values/Editors/KeycapRefEditor.h \
    Sources/Values/Editors/UserEditor.h \
    Sources/Values/Editors/UserEditorFactory.h \
    Sources/Values/Editors/UserItemDelegate.h \
    Sources/Values/Types/Finger.h \
    Sources/Values/Types/KeycapRef.h \
    Sources/Values/Types/ListValue.h

FORMS += \
    Sources/Dictionaries/DictionariesWidget.ui \
    Sources/Keyboards/KeyboardEditorMainWindow.ui \
    Sources/Keycaps/KeycapWidget.ui \
    Sources/Main/MainTabDialog.ui \
    Sources/Main/MainWindow.ui \
    Sources/Statistics/StatisticsMainWindow.ui \
    Sources/StrokesSolver/StrokesSolverWidget.ui \
    Sources/Theories/TheoryEditorMainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Installer/config/config.xml \
    Installer/packages/com.fromtonrouge.programmerstenotutor/meta/installscript.qs \
    Installer/packages/com.fromtonrouge.programmerstenotutor/meta/package.xml \
    Resources/Icons/KaladriusTrainer.png \
    Resources/Icons/application-text.png \
    Resources/Icons/arrow-curve-180-left.png \
    Resources/Icons/arrow-curve.png \
    Resources/Icons/book-brown.png \
    Resources/Icons/book-open-next.png \
    Resources/Icons/book-open.png \
    Resources/Icons/book.png \
    Resources/Icons/books-brown.png \
    Resources/Icons/books-stack.png \
    Resources/Icons/books.png \
    Resources/Icons/chain.png \
    Resources/Icons/color.png \
    Resources/Icons/control-record.png \
    Resources/Icons/cross.png \
    Resources/Icons/disk--pencil.png \
    Resources/Icons/disk.png \
    Resources/Icons/document-attribute.png \
    Resources/Icons/document.png \
    Resources/Icons/door-open-out.png \
    Resources/Icons/edit-alignment-center.png \
    Resources/Icons/edit-alignment-justify.png \
    Resources/Icons/edit-alignment-right.png \
    Resources/Icons/edit-alignment.png \
    Resources/Icons/edit-bold.png \
    Resources/Icons/edit-color.png \
    Resources/Icons/edit-indent-rtl.png \
    Resources/Icons/edit-indent.png \
    Resources/Icons/edit-italic.png \
    Resources/Icons/edit-list-order.png \
    Resources/Icons/edit-list.png \
    Resources/Icons/edit-underline.png \
    Resources/Icons/edit.png \
    Resources/Icons/folder-open-document.png \
    Resources/Icons/funnel.png \
    Resources/Icons/gear.png \
    Resources/Icons/graduation-hat.png \
    Resources/Icons/hand.png \
    Resources/Icons/image.png \
    Resources/Icons/keyboard-full.png \
    Resources/Icons/keyboard.png \
    Resources/Icons/plus.png \
    Resources/Icons/question.png \
    Resources/Icons/table.png \
    Resources/KaladriusTrainer.ico \
    Resources/Keyboards/ErgoDox.kbd \
    Resources/Keyboards/Kaladrius.kbd \
    Resources/Svgs/ergodox.svg \
    Resources/Svgs/kaladrius.svg \
    Resources/Svgs/rec-button.svg \
    Resources/Theories/ProgrammerSteno.theory \
    Sources/ProjectConfig.h.in

RESOURCES += \
    Resources/KaladriusTrainer.qrc
