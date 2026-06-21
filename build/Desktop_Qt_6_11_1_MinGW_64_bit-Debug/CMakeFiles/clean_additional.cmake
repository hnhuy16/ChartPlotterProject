# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "ChartPlugin\\CMakeFiles\\ChartPlugin_autogen.dir\\AutogenUsed.txt"
  "ChartPlugin\\CMakeFiles\\ChartPlugin_autogen.dir\\ParseCache.txt"
  "ChartPlugin\\CMakeFiles\\ChartPluginplugin_autogen.dir\\AutogenUsed.txt"
  "ChartPlugin\\CMakeFiles\\ChartPluginplugin_autogen.dir\\ParseCache.txt"
  "ChartPlugin\\CMakeFiles\\ChartPluginplugin_init_autogen.dir\\AutogenUsed.txt"
  "ChartPlugin\\CMakeFiles\\ChartPluginplugin_init_autogen.dir\\ParseCache.txt"
  "ChartPlugin\\ChartPlugin_autogen"
  "ChartPlugin\\ChartPluginplugin_autogen"
  "ChartPlugin\\ChartPluginplugin_init_autogen"
  "DemoApp\\CMakeFiles\\DemoApp_autogen.dir\\AutogenUsed.txt"
  "DemoApp\\CMakeFiles\\DemoApp_autogen.dir\\ParseCache.txt"
  "DemoApp\\DemoApp_autogen"
  )
endif()
