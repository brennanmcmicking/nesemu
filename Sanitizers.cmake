# Define the ENABLE_ASAN, ENABLE_UBSAN, ENABLE_TSAN, and ENABLE_LSAN  options.
option(ENABLE_ASAN "Enable the Address Sanitizer (ASAN)" false)
option(ENABLE_UBSAN "Enable the Undefined Behavior Sanitizer (UBSAN)" false)
option(ENABLE_TSAN "Enable the Thread Sanitzer (TSAN)" false)
option(ENABLE_LSAN "Enable the Leak Sanitzer (LSAN)" false)

# Enable ASAN, if appropriate.
if (ENABLE_ASAN)
	if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
	  CMAKE_CXX_COMPILER_ID MATCHES Clang)
		# Handle the cases of GCC and Clang.
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
		set(CMAKE_EXE_LINKER_FLAGS
		  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
	else()
		# Handle the case of unsupported compilers.
		message(FATAL_ERROR "ASAN support is unavailable.")
	endif()
endif()

# Enable UBSAN, if appropriate.
if (ENABLE_UBSAN)
	if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
	  CMAKE_CXX_COMPILER_ID MATCHES Clang)
		# Handle the cases of GCC and Clang.
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
		set(CMAKE_EXE_LINKER_FLAGS
		  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
	else()
		# Handle the case of unsupported compilers.
		message(FATAL_ERROR "UBSAN support is unavailable.")
	endif()
endif()

# Enable TSAN, if appropriate.
if (ENABLE_TSAN)
	if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
	  CMAKE_CXX_COMPILER_ID MATCHES Clang)
		# Handle the cases of GCC and Clang.
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
		set(CMAKE_EXE_LINKER_FLAGS
		  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
	else()
		# Handle the case of unsupported compilers.
		message(FATAL_ERROR "TSAN support is unavailable.")
	endif()
endif()

# Enable LSAN, if appropriate.
if (ENABLE_LSAN)
	if (CMAKE_CXX_COMPILER_ID MATCHES GNU OR
	  CMAKE_CXX_COMPILER_ID MATCHES Clang)
		# Handle the cases of GCC and Clang.
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
		set(CMAKE_EXE_LINKER_FLAGS
		  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=leak")
	else()
		# Handle the case of unsupported compilers.
		message(FATAL_ERROR "LSAN support is unavailable.")
	endif()
endif()
