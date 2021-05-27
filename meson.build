project('bandwidth_analyser', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

gtkdep = dependency('gtk+-3.0')

incdir = include_directories('/usr/include/c++/10/','/usr/include/aarch64-linux-gnu/c++/10/')

sources = [
'bandwidth_analyser.cpp',
'MyGTKWindow.cpp',
'MyGTKCalendarWindow.cpp',
'BandwidthFile.cpp',
'BandwidthDay.cpp',
'BandwidthMonth.cpp',
'BandwidthDataPoint.cpp',
'BandwidthStatistics.cpp'
]

exe = executable('bandwidth_analyser', sources,
  install : true, dependencies : gtkdep, include_directories : incdir)

test('basic', exe)
