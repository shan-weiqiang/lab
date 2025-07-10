include(cmake/CPM.cmake)
include(cmake/get_pg_cpm_functions.cmake)
set(GIT_PREFIX https://${GIT_USER_PLATFORM}:${GIT_USER_PASSWD_PLATFORM}@gitlab.phigent.net)


set(GIT_PREFIX_3RD $ENV{GIT_PREFIX_3RD})
if (NOT DEFINED ENV{GIT_PREFIX_3RD} OR "$ENV{GIT_PREFIX_3RD}" STREQUAL "")
    message(WARNING "GIT_PREFIX_3RD is not defined in ENV, so use default one")
    set(GIT_PREFIX_3RD https://gitlab+deploy-token-1:jN5Sd-Ds2F8G9C8LopKn@gitlab.phigent.net)
else ()
    message("GIT_PREFIX_3RD is set to: ${GIT_PREFIX_3RD}")
endif ()

set(SDK_URL_PREFIX $ENV{SDK_URL_PREFIX})
if (NOT DEFINED ENV{SDK_URL_PREFIX} OR "$ENV{SDK_URL_PREFIX}" STREQUAL "")
    message(WARNING "SDK_URL_PREFIX is not defined in ENV, so use default one")
    set(SDK_URL_PREFIX https://release:Release.123@artifacts.phigent.net/artifactory/psd-release/pkg)
else ()
    message("SDK_URL_PREFIX is set to: ${SDK_URL_PREFIX}")
endif ()

# psd_third_party
set(PSD_THIRDPARTY_USE_PROTOBUF ON)
# set(PSD_THIRDPARTY_USE_JSONCPP ON)
set(PSD_THIRDPARTY_USE_GLOG ON)
set(PSD_THIRDPARTY_USE_GFLAG ON)
# set(PSD_THIRDPARTY_USE_EIGEN3 ON)
# set(PSD_THIRDPARTY_USE_BOOST ON)
# set(PSD_THIRDPARTY_USE_IPOPT ON)
# set(PSD_THIRDPARTY_USE_OSQP ON)
# set(PSD_THIRDPARTY_USE_OPENCV ON)
# set(PSD_THIRDPARTY_USE_FMT ON)

PG_SET_PKG_TAG(psd_thirdparty_manager VERSION "dev_voyah_phigent" USE_SRC ON)
pg_add_package(psd_thirdparty_manager
        GIT_URL ${GIT_PREFIX_3RD}/thirdparty/psd_thirdparty_manager.git
)
include(${psd_thirdparty_manager_SOURCE_DIR}/third_party.cmake)

