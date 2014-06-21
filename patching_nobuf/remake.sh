./build.sh
rm retee
make
rm /home/jun/Desktop/ndss-evaluation/spec2006-testinputs/401.bzip2/bzip2_base.amd64-m32-gcc42-nn_new
rm /home/jun/Desktop/ndss-evaluation/spec2006-testinputs/401.bzip2/dryer.jpg.err /home/jun/Desktop/ndss-evaluation/spec2006-testinputs/401.bzip2/dryer.jpg.out
./retee /home/jun/Desktop/ndss-evaluation/spec2006-testinputs/401.bzip2/bzip2_base.amd64-m32-gcc42-nn /home/jun/Desktop/ndss-evaluation/spec2006-testinputs/401.bzip2/bzip2_base.amd64-m32-gcc42-nn_new bzip2_base_map
