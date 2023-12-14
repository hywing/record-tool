cmake_minimum_required(VERSION 3.5)

project(record-tool LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS OFF)
set(BOOST_ROOT "E:/asensing/install/lib/cmake/Boost-1.76.0")
set(PCAP_INCLUDE_DIR "E:/asensing/install/include")
set(PCAP_LIBRARY "E:/asensing/lidarview-superbuild/common-superbuild/pcap/src/Lib/x64/wpcap.lib")

include_directories(${PCAP_INCLUDE_DIR})

find_package(Qt5 COMPONENTS Widgets WebSockets REQUIRED)
find_library(PCAP_LIBRARY pcap DOC "pcap library")
find_path(PCAP_INCLUDE_DIR pcap.h DOC "pcap include directory")
find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options iostreams)

#qt5_add_resources(qrc_files resource.qrc)
qt5_wrap_ui(ui_files main_window.ui)

add_executable(${PROJECT_NAME}
    main.cpp
    main_window.cpp
    connector.cpp
    packet_receiver.cpp
    network_packet.cpp
    packet_file_writer.cpp
    pcap_writer.cpp
    streamer.cpp
    message_manager.cpp
    switch_button.cpp
#    ${qrc_files}
    ${ui_files}
    resource.rc
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::WebSockets ${PCAP_LIBRARY} ${Boost_LIBRARIES})
