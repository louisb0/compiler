as -32 test.asm -o test.o
gcc -m32 -no-pie -o test test.o
rm test.o

./test 
