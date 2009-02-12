#!/bin/bash

for a in IM VA VBO DL; do
	./grass_benchmark.mac -normals -batches 200 -it 500 -method $a
done
