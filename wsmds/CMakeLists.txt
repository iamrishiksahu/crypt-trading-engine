include(FetchContent)

# websocketpp (header-only)
FetchContent_Declare(
  websocketpp
  GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
  GIT_TAG        0.8.2
)
FetchContent_MakeAvailable(websocketpp)

# nlohmann/json (header-only)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Use standalone Asio (no Boost)
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        asio-1-30-2
)
FetchContent_MakeAvailable(asio)

add_executable(
    wsmds 
    main.cpp
    ../common/config_parser/parser.cpp
    )
target_include_directories(wsmds PRIVATE
  ${websocketpp_SOURCE_DIR}
  ${asio_SOURCE_DIR}/asio/include
)
target_link_libraries(wsmds PRIVATE nlohmann_json::nlohmann_json)

# OpenSSL for TLS
find_package(OpenSSL REQUIRED)
target_link_libraries(wsmds PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# On Linux, link pthread
if(UNIX AND NOT APPLE)
  target_link_libraries(wsmds PRIVATE pthread)
endif()
