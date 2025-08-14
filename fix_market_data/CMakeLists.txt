include(FetchContent)

# Fetch QuickFIX (C++17-ready master branch)
FetchContent_Declare(
    quickfix
    GIT_REPOSITORY https://github.com/quickfix/quickfix.git
    GIT_TAG master
)
FetchContent_MakeAvailable(quickfix)

add_executable(coinbase_fix_md main.cpp)
target_link_libraries(coinbase_fix_md PRIVATE quickfix)
