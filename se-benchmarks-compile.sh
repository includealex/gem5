compiler=arm-linux-gnueabi-gcc

test_dir="se-benchmarks"
bin_dir="arch-se-bench"
mkdir -p ${bin_dir}


for file in $(ls ${test_dir});
do
    ${compiler} ${test_dir}/${file} --static -o ${bin_dir}/${file%.*}.out
done
