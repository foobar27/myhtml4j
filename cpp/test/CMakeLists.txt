enable_testing()

find_package(Boost REQUIRED COMPONENTS system filesystem unit_test_framework)

add_executable(jniTest jniTest.cpp)

# TODO should not be necessary?
INCLUDE_DIRECTORIES(../src)

target_compile_definitions(jniTest PUBLIC BOOST_TEST_DYN_LINK) 
target_link_libraries(jniTest myhtml4jnative ${Boost_LIBRARIES})
add_test(jniTest jniTest)
