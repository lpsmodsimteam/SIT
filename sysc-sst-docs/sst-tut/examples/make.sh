#! /bin/bash

for i in $(ls -d */); do
	cd ${i%%/}
	make $1
	cd ..
done
