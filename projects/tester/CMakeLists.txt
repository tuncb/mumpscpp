project(tester CXX)
cmake_minimum_required(VERSION 3.1.2)

set(SOURCE 
  ./source/main.cpp
)

set(MUMPSCPP_LIBRARY 
  ../mumpscpp/include/mumpscpp/dmumps_c.h
  ../mumpscpp/include/mumpscpp/dmumps_root.h
  ../mumpscpp/include/mumpscpp/dmumps_struc.h
  ../mumpscpp/include/mumpscpp/EigenVector.h
  ../mumpscpp/include/mumpscpp/mumpscpp.h
  ../mumpscpp/include/mumpscpp/mumps_compat.h
  ../mumpscpp/include/mumpscpp/mumps_c_types.h
  ../mumpscpp/include/mumpscpp/UblasCoordinateAdaptor.h
  ../mumpscpp/include/mumpscpp/UblasVectorAdaptor.h
)

include(../../conan.cmake)
conan_cmake_run(BASIC_SETUP CMAKE_TARGETS CONAN_CMAKE_MULTI
  REQUIRES
  "catch2/2.2.2@bincrafters/stable"
  "msmpi/2008.2@tuncb/pangea"
  "intel-mkl/2018@tuncb/pangea"
  "boost/1.66.0@conan/stable"
  "eigen/3.3.4@conan/stable"
  "shiva/1.0.0@tuncb/pangea"
  OPTIONS
  "intel-mkl:threading=openmp"
  "intel-mkl:fortran=True"
  "intel-mkl:blacs=True"
  "intel-mkl:scalapack=True"
  "msmpi:fortran=True"
  BUILD missing
)

link_directories($ENV{ICPP_COMPILER18}\\compiler\\lib\\intel64_win)

add_executable(mumpscpp_tester ${SOURCE} ${MUMPSCPP_LIBRARY})
target_include_directories(mumpscpp_tester PRIVATE "../mumpscpp/include/")
conan_target_link_libraries(mumpscpp_tester)

target_link_libraries(mumpscpp_tester 
  debug ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/debug/dmumps_c.lib
  debug ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/debug/dmumps_fortran.lib
  debug ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/debug/mumps_common_c.lib
  debug ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/debug/pord_c.lib
  optimized ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/release/dmumps_c.lib
  optimized ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/release/dmumps_fortran.lib
  optimized ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/release/mumps_common_c.lib
  optimized ${CMAKE_CURRENT_SOURCE_DIR}/../external/lib/release/pord_c.lib
)

source_group("mumpscpp_library" FILES ${MUMPSCPP_LIBRARY})