project('bandwidth_analyser', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

gtkdep = dependency('gtk+-3.0')
thread_dep = dependency('threads')

incdir = include_directories('.','/usr/include/c++/10/','/usr/include/aarch64-linux-gnu/c++/10/')

sources = [
'bandwidth_analyser.cpp',
'MyGTKWindow.cpp',
'MyGTKCalendarWindow.cpp',
'BandwidthFile.cpp',
'BandwidthDay.cpp',
'BandwidthDataPoint.cpp',
'BandwidthStatistics.cpp',
'BandwidthData.cpp'
]

exe = executable(
'bandwidth_analyser', 
sources,
install : true, 
dependencies : [gtkdep, thread_dep], 
include_directories : incdir)

# Need to create gtest subproject by ..
# > meson wrap search gtest
# > mkdir -p subprojects
# > meson wrap install gtest
gtest = subproject('gtest')

subdir('tests')

add_test_setup('test_setup', timeout_multiplier : 4)
test('mytest', all_tests, protocol : 'gtest')
