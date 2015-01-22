set(SOA_SOURCES
    example1.cpp)

include_directories(
   ${SOA_SOURCE_DIR}/include)

set(SOA_HEADERS
    ${SOA_SOURCE_DIR}/include/soa.hh)

add_executable(example1 ${SOA_SOURCES} ${SOA_HEADERS})
target_link_libraries(example1)
