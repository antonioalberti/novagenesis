set terminal wxt size 750,300
reset
set grid
unset logscale x
unset logscale y
set key default
set xrange [46000:48000]
set yrange [0.1:0.14]
set title "Name Binding Subscription - Round Trip Delay"
set xlabel "Number of Hash Table Services"
set ylabel "Round trip delay (seconds)"
plot\
'App_Core_subrtt_test1.dat' every 1:1 using 1:3:7:8 with yerrorlines title "sub"
