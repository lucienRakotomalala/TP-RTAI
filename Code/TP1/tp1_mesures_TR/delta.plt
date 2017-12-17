set autoscale
unset log
unset label
set xtic auto
set ytic auto
set title "periode mesuree (ns)"
set xlabel "temps (periode)"
set ylabel "duree periode (ns)"
set term png
set output "delta.png"
plot "delta.res" using 1:2 title 'periode' with points
