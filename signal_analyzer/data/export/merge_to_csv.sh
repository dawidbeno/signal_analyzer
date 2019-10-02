#! /bin/bash

# Zobere data z vyexportovaneho snapshotu a appenduje do csv suboru


pocet=$1
counter=1
while [ "$counter" -le "$pocet" ]; do
	name="exportedSnapshot_"
	name+=$counter
	name+=".txt"

	newName="snapNew_"
	newName+=$counter
	newName+=".txt"

	tr "." "," < "$name" > "$newName"
	tr "\n" ";" < "$newName" > "$name"
	rm "$newName"
	
	val=$(<"$name")
	echo -n "$val" >> vlny.csv
	echo "1" >> vlny.csv


	counter=$((counter+1))
done
