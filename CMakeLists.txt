cmake_minimum_required(VERSION 2.6)
project(ib2slurm C)
set(ib2slurm_VERSION_MAJOR 0)
set(ib2slurm_VERSION_MINOR 2)

set(OFED_PREFIX "/usr" CACHE PATH
    "Install path of OFED")
if (OFED_PREFIX)
    find_path(OFED_INCLUDE_DIRS NAMES complib/cl_nodenamemap.h ibnetdisc.h
        HINTS "${OFED_PREFIX}/include" "${OFED_PREFIX}/include/infiniband"
        DOC "Path to the OFED header files")
    find_library(OFED_IBNETDISC_LIBRARY NAMES ibnetdisc
        HINTS "${OFED_PREFIX}/lib" "${OFED_PREFIX}/lib64"
        DOC "Path to the OFED libibnetdisc library")
    find_library(OFED_OSMCOMP_LIBRARY NAMES osmcomp
        HINTS "${OFED_PREFIX}/lib" "${OFED_PREFIX}/lib64"
        DOC "Path to the OFED libosmcomp library")
    find_library(OFED_IBMAD_LIBRARY NAMES ibmad
        HINTS "${OFED_PREFIX}/lib" "${OFED_PREFIX}/lib64"
        DOC "Path to the OFED libibmad library")
endif (OFED_PREFIX)

option(USE_SLURM_HOSTLISTS
    "Use SLURM's hostlist functionality to generate compact configurations" off)
if (USE_SLURM_HOSTLISTS)
    set(SLURM_PREFIX "SLURM_PREFIX-NOTFOUND" CACHE PATH
        "Install path of slurm")
    if (SLURM_PREFIX)
        find_path(SLURM_INCLUDE_DIRS slurm/slurm.h
            HINTS "${SLURM_PREFIX}/include"
            DOC "Path to the SLURM header files")
        find_library(SLURM_LIBRARIES NAMES slurm
            HINTS "${SLURM_PREFIX}/lib" "${SLURM_PREFIX}/lib64"
            DOC "Path to the SLURM libraries")
    endif (SLURM_PREFIX)
endif (USE_SLURM_HOSTLISTS)

configure_file(
    "${PROJECT_SOURCE_DIR}/ib2slurm-config.h.in"
    "${PROJECT_BINARY_DIR}/ib2slurm-config.h")
include_directories(BEFORE "${PROJECT_BINARY_DIR}")

add_executable(ib2slurm ib2slurm.c)
if (SLURM_INCLUDE_DIRS)
    include_directories(${SLURM_INCLUDE_DIRS})
endif (SLURM_INCLUDE_DIRS)
if (SLURM_LIBRARIES)
    set(LIBS ${LIBS} ${SLURM_LIBRARIES})
endif (SLURM_LIBRARIES)
if (OFED_INCLUDE_DIRS)
    include_directories(${OFED_INCLUDE_DIRS})
endif (OFED_INCLUDE_DIRS)
if (OFED_IBNETDISC_LIBRARY)
    set(LIBS ${LIBS} ${OFED_IBNETDISC_LIBRARY})
endif (OFED_IBNETDISC_LIBRARY)
if (OFED_OSMCOMP_LIBRARY)
    set(LIBS ${LIBS} ${OFED_OSMCOMP_LIBRARY})
endif (OFED_OSMCOMP_LIBRARY)
if (OFED_IBMAD_LIBRARY)
    set(LIBS ${LIBS} ${OFED_IBMAD_LIBRARY})
endif (OFED_IBMAD_LIBRARY)
target_link_libraries(ib2slurm ${LIBS})

