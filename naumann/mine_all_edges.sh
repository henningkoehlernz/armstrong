ls | grep csv | perl -pe 's/^([a-z0-9_]*)\.csv/cat $1.csv \| ..\/edgeMiner > $1_edges.txt/'
