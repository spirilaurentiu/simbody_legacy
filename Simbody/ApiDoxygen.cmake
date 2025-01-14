INCLUDE(FindDoxygen)

IF(DOXYGEN_EXECUTABLE-NOTFOUND)
ELSE(DOXYGEN_EXECUTABLE-NOTFOUND)
    SET(DOXY_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
          ${DOXY_CONFIG}
          @ONLY )

    ADD_CUSTOM_TARGET(doxygen_Simbody ${DOXYGEN_EXECUTABLE} ${DOXY_CONFIG}) 
    ADD_DEPENDENCIES(doxygen_Simbody doxygen_SimTKmath)

    FILE(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/html/")
    INSTALL(DIRECTORY "${PROJECT_BINARY_DIR}/html/"
            DESTINATION "doc/api/${PROJECT_NAME}/html"
            )
ENDIF(DOXYGEN_EXECUTABLE-NOTFOUND)

