#!/bin/bash

for a in IM VA VBO DL; do
	./grass_benchmark.mac -normals -batches 500 -it 50 -method $a
done
