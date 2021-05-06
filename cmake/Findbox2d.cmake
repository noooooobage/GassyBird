# This script locates the box2d library.
#
# Usage
# -----
#
# Use this script with:
#     find_package(box2d REQUIRED)
#
# If box2d is not installed in a standard path, then you can use the following cmake or environment
# variables to tell cmake where to find the relevant files:
#
#     Set the BOX2DINCLUDE cmake or environment variable to the path of the box2d include directory.
#     E.g. if you installed box2d to "C:/Program Files (x86)/box2d", then you should set
#     BOX2DINCLUDE to "C:/Program Files (x86)/box2d/include".
#
#     Set the BOX2DLIB cmake or environment variable to the path of the box2d lib directory. E.g.
#     if you installed box2d to "C:/Program Files (x86)/box2d", then you should set BOX2DLIB to
#     "C:/Program Files (x86)/box2d/lib".
#
# Output
# ------
#
# This script defines the following variables:
#     box2d_FOUND:       TRUE if include directory and library were able to be found
#     box2d_INCLUDE_DIR: Path where box2d header files are located
#     box2d_LIBRARY:     Path where box2d library files are installed

# list of search paths
set(box2d_search_paths
    "C:/Program Files (x86)"
    "C:/Program Files"
    /usr/local
    /usr
)

# find include directory
find_path(box2d_INCLUDE_DIR
    NAMES
        box2d/box2d.h
    PATH_SUFFIXES
        box2d/include
    PATHS
        ${BOX2DINCLUDE}
        $ENV{BOX2DINCLUDE}
        ${box2d_search_paths}
)

# find library
find_library(box2d_LIBRARY
    NAMES
        libbox2d.a
    PATH_SUFFIXES
        box2d/lib
    PATHS
        ${BOX2DLIB}
        $ENV{BOX2DLIB}
        ${box2d_search_paths}
)

# set box2d_FOUND to false if any of the necessary files/directories were not found
set(box2d_FOUND TRUE)
if (box2d_INCLUDE_DIR MATCHES "NOTFOUND$" OR box2d_LIBRARY MATCHES "NOTFOUND$")
    set(box2d_FOUND FALSE)
endif()

# output message if box2d was found
if(box2d_FOUND)
    message(STATUS "Found box2d in ${box2d_INCLUDE_DIR}")
endif()
