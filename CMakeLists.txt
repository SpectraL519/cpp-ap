# Minimalna wersja CMake wymagana do zbudowania projektu
cmake_minimum_required(VERSION 3.12)

# Nazwa projektu
project(cpp-ap)

# Ustawienia ścieżki do katalogów
set(SOURCE_DIRS "test/src" "test/app")
set(INCLUDE_DIRS "test/include" "include")
set(BINARY_DIR "test/out")
set(EXECUTABLE_DIR "test/")

# Pliki źródłowe
file(GLOB_RECURSE SOURCES "")
foreach(SOURCE_DIR ${SOURCE_DIRS})
    file(GLOB_RECURSE CURRENT_SOURCES ${SOURCE_DIR}/*.cpp)
    list(APPEND SOURCES ${CURRENT_SOURCES})
endforeach()

# Katalogi z plikami nagłówkowymi
include_directories(${INCLUDE_DIRS})

# Tworzenie celu wykonywalnego
add_executable(test ${SOURCES})

# Ustawienie ścieżki do katalogu wyjściowego
set_target_properties(test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
    CXX_STANDARD 20  # Tutaj ustawiasz żądany standard (np. 11, 14, 17, itd.)
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Ustawienie ścieżki do katalogu, w którym będą zlinkowane pliki wykonywalne
set(EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_DIR}")


# Stworzenie dowiązań symbolicznych dla plików wykonywalnych
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    file(CREATE_LINK "${BINARY_DIR}/test" "${EXECUTABLE_PATH}/test_${config}" SYMBOLIC)
endforeach(config)
