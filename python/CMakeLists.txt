PROJECT(PYLSHBOX)

FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
FIND_PACKAGE(Boost REQUIRED)

INCLUDE_DIRECTORIES(
    $ENV{Boost_DIR}
    ${PYTHON_INCLUDE_DIRS}
)

ADD_DEFINITIONS("-DLIBLINE_BUILD")

IF(MINGW)
    ADD_LIBRARY(
        pylshbox
        SHARED
        boostpy_ms.cpp
        pylshbox.cpp
    )
ELSEIF(WIN32)
    ADD_LIBRARY(
        pylshbox
        SHARED
        boostpy_vc.cpp
        pylshbox.cpp
    )
ELSE()
    ADD_LIBRARY(
        pylshbox
        SHARED
        boostpy_ms.cpp
        pylshbox.cpp
    )
ENDIF()

TARGET_LINK_LIBRARIES(
    pylshbox
    ${PYTHON_LIBRARIES}
)

SET_TARGET_PROPERTIES(
    pylshbox
    PROPERTIES
    SUFFIX
    ".pyd"
)

SET(LIBRARY_OUTPUT_PATH ${LSHBOX_BINARY_DIR}/pyd/${SAVE_CLASS})