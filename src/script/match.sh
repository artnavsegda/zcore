#!/bin/sh

keymatch() {
	keynumber=0
	for i in $1
	do
		if [ $i = $3 ]
		then
			#echo -n $i "= "
			valuenumber=0
			for y in $2
			do
				if [ $keynumber = $valuenumber ]
				then
					echo $y
				fi
				valuenumber=$((valuenumber+1))
			done
		fi
		keynumber=$((keynumber+1))
	done
}

KEY="one two three"
VALUE="raz dva tri"

keymatch "$KEY" "$VALUE" two

