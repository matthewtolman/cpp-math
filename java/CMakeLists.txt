SET_SOURCE_FILES_PROPERTIES(mtmath_c.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(mtmath_c.i PROPERTIES SWIG_FLAGS "-includeall")

SWIG_ADD_LIBRARY(javaMtMaths
        LANGUAGE java
        SOURCES mtmath_c.i)
SWIG_LINK_LIBRARIES(javaMtMaths mt-maths)

# For convenience we copy the dynamic library to the current build folder
add_custom_command(
        TARGET javaMtMaths
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:javaMtMaths> ${CMAKE_CURRENT_BINARY_DIR}
)

# Build the Java code into ExampleJNI.jar
add_jar(
        MtMathsJNI
        SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/MtMathC.java
        ${CMAKE_CURRENT_BINARY_DIR}/MtMathCJNI.java
)
add_dependencies( MtMathsJNI javaMtMaths )

set(CMAKE_JAVA_JAR_ENTRY_POINT main)
add_jar(
        JavaTest
        SOURCES Test.java
        ENTRY_POINT Test
)
add_dependencies( JavaTest MtMathsJNI )

add_custom_command(
        TARGET JavaTest
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${Java_JAVA_EXECUTABLE} -jar JavaTest.jar
)
