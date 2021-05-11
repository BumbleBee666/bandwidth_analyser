#!/bin/sh

var=`date +"%FORMAT_STRING"`
now=`date +"%Y%m%d-%H:%M:%S"`

speedtest --server-id=36718 -f csv > /home/mark/Documents/bandwidth/results_${now}.csv
