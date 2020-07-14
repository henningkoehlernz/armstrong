FILES="*_edges.txt"
for f in $FILES
do
    echo "Running informative for $f"
    cat $f | ../informative
done
