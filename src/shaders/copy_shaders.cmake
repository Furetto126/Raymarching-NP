message("Copying shaders...")

set(RESOURCES_DIR "${CMAKE_BINARY_DIR}/../../resources")
set(SHADERS_HEADER "${CMAKE_CURRENT_LIST_DIR}/ShadersContent.h")

file(GLOB SHADERS
        "${RESOURCES_DIR}/*.vert"
        "${RESOURCES_DIR}/*.frag"
)

file(WRITE ${SHADERS_HEADER})
foreach (SHADER ${SHADERS})
        get_filename_component(FILE_NAME ${SHADER} NAME_WE)
        string(TOUPPER ${FILE_NAME} FILE_NAME)
        file(READ ${SHADER} SHADER_CONTENT)

        file(APPEND ${SHADERS_HEADER} "const char *${FILE_NAME} = R\"(${SHADER_CONTENT})\";\n\n")
endforeach ()

message("Shaders copied")