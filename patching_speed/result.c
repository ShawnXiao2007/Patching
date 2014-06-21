//signal sync + buf change 6/6/2013
gaoyao@gaoyao-laptop:~/dyninst/mutator/patching$ \time ./gzip_2 -d /home/gaoyao/vm_share/ion.QMV.gz 
1.07user 8.38system 0:17.08elapsed 55%CPU (0avgtext+0avgdata 359280maxresident)k
0inputs+0outputs (0major+22500minor)pagefaults 0swaps
gaoyao@gaoyao-laptop:~/dyninst/mutator/patching$ \time ./gzip_2 /home/gaoyao/vm_share/ion.QMV
16.36user 15.72system 0:39.04elapsed 82%CPU (0avgtext+0avgdata 3808maxresident)k
0inputs+0outputs (0major+284minor)pagefaults 0swaps
gaoyao@gaoyao-laptop:~/dyninst/mutator/patching$ \time ./gzip -d /home/gaoyao/vm_share/ion.QMV.gz 
0.72user 7.44system 0:14.40elapsed 56%CPU (0avgtext+0avgdata 2576maxresident)k
0inputs+0outputs (0major+198minor)pagefaults 0swaps
gaoyao@gaoyao-laptop:~/dyninst/mutator/patching$ \time ./gzip /home/gaoyao/vm_share/ion.QMV
15.77user 15.99system 0:38.78elapsed 81%CPU (0avgtext+0avgdata 3088maxresident)k
0inputs+0outputs (0major+230minor)pagefaults 0swaps
