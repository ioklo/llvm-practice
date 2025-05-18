cmake_minimum_required(VERSION 3.23)
project(03_Execution)

set(CMAKE_CXX_STANDARD 17)

# LLVM 경로 설정 (필요 시 수동 설정 가능)
# set(LLVM_DIR "/usr/local/opt/llvm/lib/cmake/llvm") # macOS (Homebrew)
# set(LLVM_DIR "/Users")       # 수동 빌드

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# LLVM 설정 반영
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# 사용할 LLVM 컴포넌트 지정
#llvm_map_components_to_libnames(LLVM_LIBS support core irreader)

# 실행파일 정의
add_executable(MyLLVMTool 03_Execution.cpp)

llvm_config(MyLLVMTool USE_SHARED support core irreader)

# LLVM 링크
target_link_libraries(MyLLVMTool PRIVATE ${LLVM_LIBS})