# 요구 CMake 최소 버전
cmake_minimum_required(VERSION 3.0)

# 프로젝트 이름 및 버전
project("md5Chekcer"
  VERSION   1.0.0
  LANGUAGES CXX C
)

# 주절주절 Makefile 생성 여부
set(CMAKE_VERBOSE_MAKEFILE false)

# CMake가 빌드할 운영체제 이름 및 버전 정보 확인
message(STATUS "CMAKE_SYSTEM: ${CMAKE_SYSTEM}")

# Debug 빌드인지 Release 빌드인지 확인
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 32-bit인지 64-bit인지 확인 후 ARCHTECTURE 설정
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(PROJ_ARCH "32")
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PROJ_ARCH "64")
endif()

# 운영체제별 라이브러리 검색(혹은 로그출력)
if(UNIX AND ${CMAKE_SYSTEM} MATCHES Linux)
  set(PROJ_TARGET_PLATFORM "Linux")

  # pthread
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  # pthread가 없는 경우
  if(NOT Threads_FOUND)
    message(FATAL_ERROR "Can't find the essential libraries. Please install them.")
  endif()
elseif(${CMAKE_SYSTEM} MATCHES Darwin) # Darwin = Macos
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(PROJ_TARGET_PLATFORM "Mac")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PROJ_TARGET_PLATFORM "Mac")
  endif()
endif()

# C++ 플래그 설정 (C++17)
set(CMAKE_CXX_STANDARD 17)

# -fPIC 플래그 설정
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


###############################################################################
# 빌드 대상 파일명
###############################################################################
## Project C++ Libraries
set(CPP_MD5CHECKER_LIB             "libmd5Checker")

## Test Applications
set(APP_MD5CHECKER_EXE             "md5checker")

## 파일 출력 경로 확인
set(PROJ_OUTPUT_DIRECTORY ${PROJ_TARGET_PLATFORM}/${CMAKE_BUILD_TYPE})
message(STATUS "PROJ_OUTPUT_DIRECTORY: ${PROJ_OUTPUT_DIRECTORY}")


###############################################################################
# 빌드 대상 소스 파일 목록
###############################################################################
## Project C++ sources
set(CPP_MD5CHECKER_SRC
  src/md5Checker/md5Checker.cpp
)

## Project Applications
set(APP_MD5CHECKER_SRC
  test/md5Checker_main.cpp
)


###############################################################################
# 공통 헤더 파일 include 디렉토리 (-I)
###############################################################################
include_directories(
  inc/md5Checker
)


###############################################################################
# 컴파일 플래그 정의 추가
###############################################################################
## 공통 플래그
#add_definitions(
#  -D_MZ_USE_CUSTOM_AUDIO=1
#  -D_MZ_USE_CSDK=1
#  -DRAPIDJSON_HAS_STDSTRING=1
#)

### 플랫폼에 따른 플래그
if(WIN32)
  add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
    -D_MZ_SE_NUANCE_VOCALIZER_VERSION=350
    -D_MZ_WRAP_CONFIG_ENVIRONMENT=0x21020000
    -D_WIN32_WINNT=0x0601
    -DWIN32
  )
elseif(${CMAKE_SYSTEM} MATCHES Linux)
  add_definitions(
    -D_MD5_BUILD_SYSTEM=0x0001
  )
elseif(${CMAKE_SYSTEM} MATCHES Darwin)
  add_definitions(
    -D_MD5_BUILD_SYSTEM=0x0002
  )
endif()


###############################################################################
# 추가 라이브러리 링크
###############################################################################
#link_directories(
#
#)
#
#link_libraries(
#
#)


###############################################################################
# 출력 디렉토리 설정
###############################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/${PROJ_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib/${PROJ_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib/${PROJ_OUTPUT_DIRECTORY})


###############################################################################
# 컴파일 옵션 및 링크 옵션 출력 (확인용)
###############################################################################
message(STATUS "CMAKE_C_FLAGS_DEBUG:            ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CMAKE_C_FLAGS_RELEASE:          ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO:   ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_C_FLAGS_MINSIZEREL:       ${CMAKE_C_FLAGS_MINSIZEREL}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG:          ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE:        ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_CXX_FLAGS_MINSIZEREL:     ${CMAKE_CXX_FLAGS_MINSIZEREL}")


###############################################################################
# 라이브러리 소스 파일들을 정적 라이브러리로 추가
###############################################################################
add_library(
  ${CPP_MD5CHECKER_LIB}
  STATIC
  ${CPP_MD5CHECKER_SRC}
)


###############################################################################
# 실행 바이너리 소스 파일들을 실행 바이너리로 추가
###############################################################################
add_executable(
  ${APP_MD5CHECKER_EXE}
  ${APP_MD5CHECKER_SRC}
)


###############################################################################
# 특정 Target 전처리기 매크로 정의 (-D)
###############################################################################
#target_compile_definitions(
#  ${TIMA_C_MZ_SE_LIB}
#  PRIVATE
#  -D_MZ_USE_CSDK_ASR=1
#  -DTIMA_USE_NAMZ_MKASR=1
#)


###############################################################################
# 특정 Target 헤더 디렉토리 추가 (-I)
###############################################################################
target_include_directories(
  ${APP_MD5CHECKER_EXE}
  PRIVATE
  inc/md5Checker
)


###############################################################################
# Target 전처리기 매크로 정의 (-D)
###############################################################################
# Summary of tuple support for Microsoft Visual Studio:
# Compiler    version(MS)  version(cmake)  Support
# ----------  -----------  --------------  -----------------------------
# <= VS 2010  <= 10        <= 1600         Use Google Tests's own tuple.
# VS 2012     11           1700            std::tr1::tuple + _VARIADIC_MAX=10
# VS 2013     12           1800            std::tr1::tuple
# VS 2015     14           1900            std::tuple
# VS 2017     15           >= 1910         std::tuple
#if(MSVC AND MSVC_VERSION EQUAL 1800)
#  target_compile_definitions(
#      ${TIMA_APP_UNIT_TEST_EXE}
#      PRIVATE
#      _VARIADIC_MAX=10
#    )
#endif()


#########################################
# 특정 Target 링크 옵션 및 라이브러리 지정 (-l)
###############################################################################

## 라이브러리간 의존성 지정
target_link_libraries(
  ${APP_MD5CHECKER_EXE}
  PRIVATE
  ${CPP_MD5CHECKER_LIB}
)

if(${CMAKE_SYSTEM} MATCHES Linux)
  target_link_libraries(
    ${APP_MD5CHECKER_EXE}
    PRIVATE
    Threads::Threads
  )
endif()