gcc -fpic -c PtrDeref.c
echo "****.o"
gcc -shared -o libPtrDeref.so PtrDeref.o
echo "****.so"
cp libPtrDeref.so /usr/lib/
ls /usr/lib/libPtrDeref.so -l
echo "****cp to /user/lib"

