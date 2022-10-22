
SET_SOURCE_FILES_PROPERTIES(mtmath_c.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(mtmath_c.i PROPERTIES SWIG_FLAGS "-includeall")

SWIG_ADD_LIBRARY(pyMtMaths
        LANGUAGE python
        SOURCES mtmath_c.i)
SWIG_LINK_LIBRARIES(pyMtMaths ${PYTHON_LIBRARIES} mt-maths)

add_custom_command(
        TARGET pyMtMaths
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
        TARGET pyMtMaths
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${Python3_EXECUTABLE} test.py
)
