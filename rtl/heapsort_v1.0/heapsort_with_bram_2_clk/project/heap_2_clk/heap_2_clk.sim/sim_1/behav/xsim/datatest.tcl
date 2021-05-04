set n 1
set f [open ./res.dat w]
close $f
set f [open ./fault.dat w]
close $f
while {$n < 100} {
	set f [open ./num.dat w]
	set hex [format "%16x" $n]
	puts $f $hex
	close $f
	launch_simulation
	run all
	close_sim
	set result [exec python test.py]
	set f [open ./res.dat a]
	puts $f $result
	close $f
	if {$result == 0} {
		set f [open ./fault.dat a]
		puts $f $n
		close $f
	}
	incr n
}

