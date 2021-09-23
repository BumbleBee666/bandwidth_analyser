# bandwidth_analyser
bandwidth.sh run repeatedly using cron job..
*/5 * * * * /home/mark/Documents/bandwidth/bandwidth.sh

Creates files with bandwidth metrics.

C++ app uses GTK v3.0 to create window and display graph of monthly average speeds during 24h period.

Currently the month is hardcoded.  As is the use of month for the graph.
Improvements would be to display daily actual speeds rather than average over month.
Obviously allowing selection of month from app would make sense.
