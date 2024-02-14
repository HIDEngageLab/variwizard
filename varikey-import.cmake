if (DEFINED ENV{VARIKEY_FETCH_FROM_GIT_PATH} AND (NOT VARIKEY_FETCH_FROM_GIT_PATH))
    set(VARIKEY_FETCH_FROM_GIT_PATH $ENV{VARIKEY_FETCH_FROM_GIT_PATH})
    message("Using VARIKEY_FETCH_FROM_GIT_PATH from environment ('${VARIKEY_FETCH_FROM_GIT_PATH}')")
endif ()
set(VARIKEY_FETCH_FROM_GIT_PATH "${VARIKEY_FETCH_FROM_GIT_PATH}" CACHE FILEPATH "location to download varikey firmware")

include(FetchContent)
set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
if (VARIKEY_FETCH_FROM_GIT_PATH)
    get_filename_component(FETCHCONTENT_BASE_DIR "${VARIKEY_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
endif ()

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17.0")
    FetchContent_Declare(
            varikey
            GIT_REPOSITORY https://github.com/HIDEngageLab/varikey
            GIT_TAG main
            GIT_SUBMODULES_RECURSE FALSE
    )
else ()
    FetchContent_Declare(
            varikey
            GIT_REPOSITORY https://github.com/HIDEngageLab/varikey
            GIT_TAG main
    )
endif ()

FetchContent_GetProperties(varikey)
if (NOT varikey_POPULATED)
    message("Downloading varikey firmware source")
    FetchContent_Populate(varikey)
    #FetchContent_MakeAvailable(varikey)
    #set(VARIKEY_PATH ${varikey_SOURCE_DIR})
    #add_subdirectory(${varikey_SOURCE_DIR} ${varikey_BINARY_DIR})
    message("source ${varikey_SOURCE_DIR}")
endif ()
set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})
