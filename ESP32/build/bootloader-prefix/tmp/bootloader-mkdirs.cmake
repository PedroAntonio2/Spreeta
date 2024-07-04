# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/PedroAntonio/esp/v5.2.2/esp-idf/components/bootloader/subproject"
  "/home/PedroAntonio/SpreetaESP32/build/bootloader"
  "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix"
  "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix/tmp"
  "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix/src/bootloader-stamp"
  "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix/src"
  "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/PedroAntonio/SpreetaESP32/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
