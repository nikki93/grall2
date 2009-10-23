#!/bin/sh

#mv data/Python/pyo/* data/Python
#rm -rf data/Python/pyo

python -O -m compileall data/Python

#cd data/Python
#mkdir pyo
#mv *.pyo pyo
