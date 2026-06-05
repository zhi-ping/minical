include(FetchContent)

set(BENCHMARK_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE) # 这是一个全局开关，很多库都认它

# 封装一个函数或直接编写逻辑
macro(setup_dependencies)
    message(STATUS "Setting up local dependencies from 3rd...")
    # --- 1. 禁止 GoogleTest 生成它自己的测试 ---
    # GTest 本身有很多测试，我们只需要库
    FetchContent_Declare(
      googletest
      URL "${CMAKE_CURRENT_SOURCE_DIR}/3rd/googletest-1.17.0.zip"
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # --- 2. 禁止 Google Benchmark 生成它自己的测试和文档 ---
    # Benchmark 默认会生成一堆测试用例
    FetchContent_Declare(
      google_benchmark
      URL "${CMAKE_CURRENT_SOURCE_DIR}/3rd/benchmark-1.9.5.zip"
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(BENCHMARK_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(google_benchmark)
    
endmacro()