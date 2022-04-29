# Install script for directory: /Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/cmake-build-release/3rd-party/JUCE/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/cmake-build-release/3rd-party/JUCE/extras/Build/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-6.1.6" TYPE FILE FILES
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/cmake-build-release/3rd-party/JUCE/JUCEConfigVersion.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/cmake-build-release/3rd-party/JUCE/JUCEConfig.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/JUCECheckAtomic.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/JUCEHelperTargets.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/JUCEModuleSupport.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/JUCEUtils.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/LaunchScreen.storyboard"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/PIPComponent.cpp.in"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/PIPConsole.cpp.in"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/checkBundleSigning.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/copyDir.cmake"
    "/Users/shanjiang/Documents/GitHub/MUSI6106-Project/plugin_framework/ProcessorGraphTest/3rd-party/JUCE/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    )
endif()

