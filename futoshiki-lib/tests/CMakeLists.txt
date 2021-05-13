# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.9.1)
FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

add_executable(testtarget main.cpp)

target_compile_features(testtarget PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(testtarget PRIVATE futoshiki_library Catch2::Catch2)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME testfuto COMMAND testtarget) # Command can be a target
