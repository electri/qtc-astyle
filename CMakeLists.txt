add_subdirectory(3rdparty/astyle)

add_qtc_plugin(Astyle
  DEPENDS
    Qt::Concurrent
    Qt::Widgets
    AstyleLib
  PLUGIN_DEPENDS
    Core
    ProjectExplorer
    TextEditor
  DEFINES
    ASTYLE_LIB
    ASTYLE_NO_EXPORT
  SYSTEM_INCLUDES
    3rdparty/astyle
  SOURCES
    astyle.qrc
    astyleconstants.h
    astyleformatter.cpp astyleformatter.h
    astyleoptionhelp.cpp astyleoptionhelp.h
    astyleplugin.cpp
    astyleprojectdialog.cpp astyleprojectdialog.h
    astylesettings.cpp astylesettings.h
    astyletr.h
)
