PROJECT(MATLSHBOX)

SET(MEX_NAME_SET
    rhplsh_search
    thlsh_search
    psdlsh_search
    shlsh_search
    itqlsh_search
    dbqlsh_search
    kdbqlsh_search
)

IF(WIN32)
    FIND_PROGRAM(MEX_CMD mex.bat)
    FIND_PROGRAM(MEXEXT_CMD mexext.bat)
ELSE()
    FIND_PROGRAM(MEX_CMD mex)
    FIND_PROGRAM(MEXEXT_CMD mexext)
ENDIF()

IF(MEX_CMD AND MEXEXT_CMD)

    GET_FILENAME_COMPONENT(MEX_REAL_CMD ${MEX_CMD} ABSOLUTE)
    GET_FILENAME_COMPONENT(MEX_PATH ${MEX_REAL_CMD} PATH)

    GET_FILENAME_COMPONENT(MEXEXT_REAL_CMD ${MEXEXT_CMD} ABSOLUTE)
    GET_FILENAME_COMPONENT(MEXEXT_PATH ${MEXEXT_REAL_CMD} PATH)

    IF(MEX_PATH STREQUAL MEXEXT_PATH)
        MESSAGE(STATUS "Found MATLAB at: " ${MEX_PATH})

        EXECUTE_PROCESS(COMMAND ${MEXEXT_REAL_CMD} OUTPUT_VARIABLE MEX_EXTENSION OUTPUT_STRIP_TRAILING_WHITESPACE)

        FOREACH(MEX_NAME ${MEX_NAME_SET})

            SET(MEX_FILE ${MATLSHBOX_BINARY_DIR}/${MEX_NAME}.${MEX_EXTENSION})

            ADD_CUSTOM_COMMAND(
                OUTPUT ${MEX_FILE}
                COMMAND ${MEX_REAL_CMD}
                ARGS ${MATLSHBOX_SOURCE_DIR}/${MEX_NAME}.cpp -I${LSHBOX_SOURCE_DIR}/include
                COMMENT "Building MEX extension ${MEX_FILE}"
            )
            ADD_CUSTOM_TARGET(mex_${MEX_NAME} ALL DEPENDS ${MEX_FILE})
        ENDFOREACH(MEX_NAME)
    ELSE()
        MESSAGE(SEND_ERROR
            "The 'mex' and 'mexext' programs have been found in different locations. It's likely that one of them is not part of the MATLAB instalation. Make sure that the 'bin' directory from the MATLAB instalation is in PATH"
        )
    ENDIF()
ELSE()
    MESSAGE(SEND_ERROR
        "Cannot find MATLAB or Octave instalation. Make sure that the 'bin' directory from the MATLAB instalation or that mkoctfile is in PATH"
    )
ENDIF()