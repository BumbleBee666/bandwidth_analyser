all_test_deps = []
all_test_dep_libs = []
project_test_sources = [
'../BandwidthFile.cpp',
'../BandwidthDay.cpp',
'../BandwidthDataPoint.cpp',
'../BandwidthStatistics.cpp',
'../BandwidthData.cpp'
]

gtest_dep = gtest.get_variable('gtest_dep')
thread_dep = dependency('threads')
test_main = files('test_BandwidthData.cpp')

test_deps = [
    gtest_dep, 
    thread_dep
]

test_dep_libs = [

]

# This executable contains all the tests
project_test_sources += test_main
all_test_deps += test_deps
all_test_dep_libs += test_dep_libs

all_tests = executable('all_tests', 
    project_test_sources, 
    include_directories : incdir, 
    dependencies : all_test_deps, 
    link_with : all_test_dep_libs)

