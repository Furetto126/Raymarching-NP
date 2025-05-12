set(SHADERS_HEADER "${CMAKE_CURRENT_LIST_DIR}/ShadersContent.h")

if (NOT EXISTS ${SHADERS_HEADER})
        message("Copied shaders not found, forcing copy...")
        execute_process(COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/force-shader-copy.stamp")
endif ()