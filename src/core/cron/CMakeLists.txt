# Copyright (c) Monetas AG, 2014

set(cxx-sources
  OTCron.cpp
  OTCronItem.cpp
)

file(GLOB cxx-install-headers "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/opentxs/core/cron/*.hpp")

set(cxx-headers
  ${cxx-install-headers}
)

set(dependency_include_dir
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../deps/
)

include_directories(SYSTEM
  ${dependency_include_dir}
)

set(MODULE_NAME opentxs-cron)
add_library(${MODULE_NAME}
  OBJECT
  ${cxx-sources}
  ${cxx-headers}
)

install(FILES ${cxx-install-headers}
        DESTINATION include/opentxs/core/cron
        COMPONENT dev)

set_property(TARGET ${MODULE_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
