PROJECT(TOOLS)

SET(TOOLS
    create_test_data
    create_benchmark
    rbslsh_test
    rhplsh_test
    thlsh_test
    psdlsh_test
    shlsh_test
    itqlsh_test
    dbqlsh_test
    kdbqlsh_test
)

FOREACH(TOOL ${TOOLS})
    ADD_EXECUTABLE(${TOOL} ${TOOL}.cpp)
ENDFOREACH(TOOL)

SET(EXECUTABLE_OUTPUT_PATH ${LSHBOX_BINARY_DIR}/bin/${SAVE_CLASS})