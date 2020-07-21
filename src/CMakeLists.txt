add_executable (padosuso
main.cpp
Map.cpp
Statek.cpp
)

target_link_libraries(padosuso
rtmidi
)