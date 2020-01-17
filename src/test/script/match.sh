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

echo matching input $1 argument to $(keymatch "$_NAMESAKES" "$_SECTIONS" $1) value

