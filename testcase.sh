[ -d "./test/" ] && rm -rf ./test/
mkdir test

# 初始化寫入 10000 行
for i in {1..10000}; do 
    echo $i >> /tmp/ssd/ssd_file
done

./ssd_fuse_dut /tmp/ssd/ssd_file r 80000 > ./test/ssd_dump_0.txt

# 將 100 個數據寫入，以 $i * 100 為偏移量
for i in {1..100}; do
    ./ssd_fuse_dut /tmp/ssd/ssd_file w 10 $(($i * 100)) 
done
./ssd_fuse_dut /tmp/ssd/ssd_file r 80000 > ./test/ssd_dump_1.txt

# 寫入 10 個數據，以 $i * 981 為偏移量
for i in {1..10}; do 
    ./ssd_fuse_dut /tmp/ssd/ssd_file w 981 $(($i * 981))
done
./ssd_fuse_dut /tmp/ssd/ssd_file r 80000 > ./test/ssd_dump_2.txt

# 測試檔案的寫入
for i in {1..10000}; do 
    echo $i >> ./test/test.txt
done
./ssd_fuse_dut ./test/test.txt r 80000 > ./test/test_dump_0.txt

echo 1

# 將 100 個數據寫入，以 $i * 100 為偏移量
for i in {1..100}; do 
    ./ssd_fuse_dut ./test/test.txt w 10 $(($i * 100)) 
done
./ssd_fuse_dut ./test/test.txt r 80000 > ./test/test_dump_1.txt

echo 2

# 寫入 10 個數據，以 $i * 981 為偏移量
for i in {1..10}; do 
    ./ssd_fuse_dut ./test/test.txt w 981 $(($i * 981)) 
done
./ssd_fuse_dut ./test/test.txt r 80000 > ./test/test_dump_2.txt

# 比較差異
for i in {0..2}; do 
    diff -s ./test/ssd_dump_$i.txt ./test/test_dump_$i.txt
done