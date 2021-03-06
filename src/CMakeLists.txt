include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-fvisibility=hidden __HAVE_GCC_VISIBILITY)
if (__HAVE_GCC_VISIBILITY AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif (__HAVE_GCC_VISIBILITY AND NOT WIN32)

#include(${QT_USE_FILE})
include(QZeitgeistMacros)

execute_process(COMMAND python -c "import zeitgeist.datamodel" RESULT_VARIABLE ZEITGEIST_FIND_RESULT)

if (NOT (ZEITGEIST_FIND_RESULT EQUAL 0))
    message(FATAL_ERROR "Zeitgeist python API not found. Please install zeitgeist.")
endif()

add_definitions(-DBUILD_QZEITGEIST)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

set(qzeitgeist_HEADERS
    datasourceregistry.h
    log.h
    logmodel.h
    logbrowser.h
    monitor.h
    refreshjob.h
    qzeitgeist.h
    ${CMAKE_CURRENT_BINARY_DIR}/qzeitgeist_version.h
)

set(qzeitgeist_MODEL_HEADERS
    DataModel/datasource.h
    DataModel/event.h
    DataModel/subject.h
    DataModel/timerange.h
)

set(qzeitgeist_SRCS
    datasourceregistry.cpp
    log.cpp
    logbrowser.cpp
    logmodel.cpp
    monitor.cpp
    refreshjob.cpp
    DataModel/datasource.cpp
    DataModel/event.cpp
    DataModel/subject.cpp
    DataModel/timerange.cpp
)

configure_file(qzeitgeist_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/qzeitgeist_version.h @ONLY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/DataModel ${CMAKE_CURRENT_BINARY_DIR}/DataModel)

set(zeitgeist_DBUS_XML
    dbus-xml/org.gnome.zeitgeist.Log.xml
    dbus-xml/org.gnome.zeitgeist.Blacklist.xml
    dbus-xml/org.gnome.zeitgeist.DataSourceRegistry.xml
)

set_source_files_properties(${zeitgeist_DBUS_XML} PROPERTIES INCLUDE QZeitgeist5/QZeitgeist)
QT5_ADD_DBUS_INTERFACE(qzeitgeist_SRCS dbus-xml/org.gnome.zeitgeist.Log.xml loginterface)
QT5_ADD_DBUS_INTERFACE(qzeitgeist_SRCS dbus-xml/org.gnome.zeitgeist.Blacklist.xml blacklistinterface)
QT5_ADD_DBUS_INTERFACE(qzeitgeist_SRCS dbus-xml/org.gnome.zeitgeist.DataSourceRegistry.xml datasourceregistryinterface)

qt5_add_dbus_adaptor(qzeitgeist_SRCS dbus-xml/org.gnome.zeitgeist.Monitor.xml monitor_p.h QZeitgeist::MonitorPrivate)

add_library(qzeitgeist5 SHARED ${qzeitgeist_SRCS} interpretation.h manifestation.h)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/interpretation.h
   COMMAND ${CMAKE_SOURCE_DIR}/scripts/onto2cpp.py -o Interpretation > ${CMAKE_CURRENT_BINARY_DIR}/interpretation.h
                   )

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/manifestation.h
   COMMAND ${CMAKE_SOURCE_DIR}/scripts/onto2cpp.py -o Manifestation > ${CMAKE_CURRENT_BINARY_DIR}/manifestation.h
                   )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifestation.h ${CMAKE_CURRENT_BINARY_DIR}/interpretation.h DESTINATION include/QZeitgeist5)

target_link_libraries(qzeitgeist5 ${QT_QTDBUS_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QDBUS_LDFLAGS})
set_target_properties(qzeitgeist5 PROPERTIES VERSION "${QZEITGEIST_VERSION}"
                      VERSION ${QZEITGEIST_VERSION}
                      SOVERSION ${QZEITGEIST_LIB_SOVERSION}
                      )

install(TARGETS qzeitgeist5 EXPORT QZeitgeistExport DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
qt5_use_modules(qzeitgeist5 Core DBus Test Declarative Widgets)

install(FILES ${qzeitgeist_HEADERS} DESTINATION include/QZeitgeist5)
install(FILES ${qzeitgeist_MODEL_HEADERS} DESTINATION include/QZeitgeist5/DataModel)
