cmake_minimum_required(VERSION 2.6)
project(depends)
enable_testing()

option(ENABLE_SERIALIZATION "Enable serialization using Boost.Serialization" OFF)

if (ENABLE_SERIALIZATION)
	find_package(Boost
		REQUIRED
		COMPONENTS serialization
		)
	add_definitions(-DDEPENDS_SUPPORT_SERIALIZATION)
endif()

set(TESTS
	dag
	depends
	serialize_dag
	serialize_depends
	)

foreach(test ${TESTS})
	add_executable(test_${test} tests/${test}.cpp)
	add_test(test_${test} ${EXECUTABLE_OUTPUT_PATH}/test_${test})

	if (ENABLE_SERIALIZATION)
		add_executable(test_ser_${test} tests/${test}.cpp)
		add_test(test_ser_${test} ${EXECUTABLE_OUTPUT_PATH}/test_ser_${test})
		target_link_libraries(test_ser_${test} ${Boost_SERIALIZATION_LIBRARY})
		target_link_libraries(test_${test} ${Boost_SERIALIZATION_LIBRARY})
	endif()
endforeach()
