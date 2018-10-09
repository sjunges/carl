ExternalProject_Add(
	Z3-EP
	GIT_REPOSITORY "https://github.com/Z3Prover/z3.git"
	GIT_TAG z3-${Z3_VERSION}
	UPDATE_COMMAND ""
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_INSTALL_LIBDIR=lib
	BUILD_COMMAND ${CMAKE_COMMAND} -DBUILD_LIBZ3_SHARED=ON <SOURCE_DIR>
	COMMAND ${CMAKE_MAKE_PROGRAM} libz3
	COMMAND ${CMAKE_MAKE_PROGRAM} install
	COMMAND ${CMAKE_COMMAND} -DBUILD_LIBZ3_SHARED=OFF <SOURCE_DIR>
	COMMAND ${CMAKE_MAKE_PROGRAM} libz3
	COMMAND ${CMAKE_MAKE_PROGRAM} install
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(Z3-EP INSTALL_DIR)
ExternalProject_Get_Property(Z3-EP SOURCE_DIR)

add_imported_library(Z3 SHARED "${INSTALL_DIR}/lib/libz3${DYNAMIC_EXT}" "${SOURCE_DIR}/src")
add_imported_library(Z3 STATIC "${INSTALL_DIR}/lib/libz3${STATIC_EXT}" "${SOURCE_DIR}/src")

add_dependencies(Z3_SHARED Z3-EP)
add_dependencies(Z3_STATIC Z3-EP)

add_dependencies(resources Z3_SHARED)
add_dependencies(resources Z3_STATIC)
