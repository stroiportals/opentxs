# Copyright (c) Monetas AG, 2014

add_subdirectory(contract)
add_subdirectory(cron)
add_subdirectory(crypto)
add_subdirectory(otprotob)
add_subdirectory(recurring)
add_subdirectory(script)
add_subdirectory(trade)
add_subdirectory(transaction)
add_subdirectory(util)

set(cxx-sources
  Account.cpp
  AccountList.cpp
  Cheque.cpp
  Contract.cpp
  Data.cpp
  Identifier.cpp
  Instrument.cpp
  Item.cpp
  Ledger.cpp
  Log.cpp
  Message.cpp
  NumList.cpp
  Nym.cpp
  NymIDSource.cpp
  OTStorage.cpp
  OTStringXML.cpp
  OTTrackable.cpp
  OTTransaction.cpp
  OTTransactionType.cpp
  String.cpp
)

file(GLOB cxx-install-headers
     "${CMAKE_CURRENT_SOURCE_DIR}/../../include/opentxs/core/*.hpp"
)

set(cxx-headers
  ${cxx-install-headers}
)

include_directories(${ProtobufIncludePath})

if(KEYRING_GNOME)
  include(GNUInstallDirs)
  include_directories(${CMAKE_INSTALL_FULL_OLDINCLUDEDIR}/glib-2.0/ /usr/${CMAKE_INSTALL_LIBDIR}/glib-2.0/include/ ${CMAKE_INSTALL_FULL_OLDINCLUDEDIR}/gnome-keyring-1/)
endif()

include_directories(SYSTEM
  ${OPENSSL_INCLUDE_DIR}
  ${PROTOBUF_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIRS}
)

if (LIBSECP256K1_EXPORT AND NOT OT_INTERNAL_LIBSECP)
  include_directories(SYSTEM ${LIBSECP256K1_INCLUDE_DIR})
endif()

set(MODULE_NAME opentxs-core)
if(WIN32)
  # suppress warnings about exported internal symbols (global log stream objects)
  add_definitions(-DOTLOG_IMPORT)

  # suppress linker warnings about locallly defined imported symbols
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4049 /ignore:4217")

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/windll.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/module.rc
    @ONLY
  )

  # we have an extra Windows-specific file to compile
  add_library(${MODULE_NAME} OBJECT
    ${cxx-sources}
    ${cxx-headers}
    util/OTWindowsRegistryTools.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/module.rc
  )
else()
  add_library(${MODULE_NAME} OBJECT
    ${cxx-sources}
    ${cxx-headers}
  )
endif()

install(FILES ${cxx-install-headers}
        DESTINATION include/opentxs/core
        COMPONENT dev)

set_property(TARGET ${MODULE_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
set_lib_property(${MODULE_NAME})

add_dependencies(${MODULE_NAME} otprotob)
