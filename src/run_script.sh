make clean
make
rm -f custom1.txt
echo "Tournament plus Gshare" > custom1.txt
echo "========" >> custom1.txt
echo "========" >> custom1.txt
echo "INT_1" >> custom1.txt
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom1 >> custom1.txt
echo "========" >> custom1.txt
echo "========" >> custom1.txt
echo "INT_2" >> custom1.txt
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom1 >> custom1.txt 
echo "========" >> custom1.txt
echo "========" >> custom1.txt
echo "FP_1" >> custom1.txt
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom1  >> custom1.txt
echo "========" >> custom1.txt
echo "========" >> custom1.txt
echo "FP_2" >> custom1.txt
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom1  >> custom1.txt
echo "========" >> custom1.txt
echo "========" >> custom1.txt
echo "MM_1" >> custom1.txt
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom1  >> custom1.txt
echo "========" >> custom1.txt
echo "========" >> custom1.txt
echo "MM_2" >> custom1.txt
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom1  >> custom1.txt
echo "========" >> custom1.txt