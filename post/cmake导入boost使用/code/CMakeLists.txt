cmake_minimum_required(VERSION 3.28)
project(cmake_use_boost)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置三方库存放目录为 项目根目录/thirdparty
set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)
message("thirdparty_dir: " ${THIRDPARTY_DIR})

include(FetchContent)
# 显示下载进度
set(FETCHCONTENT_QUIET OFF)

# import boost
# 设置需要使用的 boost 库
set(BOOST_INCLUDE_LIBRARIES bimap)
set(BOOST_ENABLE_CMAKE ON)
FetchContent_Declare(
    boost
    # boost release 下载地址
    URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.gz
	URL_HASH MD5=c62ce6e64d34414864fef946363db91cea89c1b90360eabed0515f0eda74c75c
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    # 下载后解压到本地的目录
    SOURCE_DIR ${THIRDPARTY_DIR}/boost
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(boost)

add_executable(main main.cc)
# 链接 boost 库
target_link_libraries(main boost_bimap)

