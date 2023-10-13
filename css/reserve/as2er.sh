#!/bin/sh
for file in *.css
do
	sed s/as-blog/er-blog/g < $file > $file.new
	mv $file $file.old
	mv $file.new $file
done
