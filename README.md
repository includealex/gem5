## M5_PATH creation for legacy issues:

```bash
# From the `gem5/' directory:
mkdir -p full_system/disks
mkdir -p full_system/binaries
export M5_PATH="${PWD}/full_system/" >> ~/.bashrc
source ~/.bashrc
```

## Disk installation
```bash
wget -P "${M5_PATH}/disks" http://dist.gem5.org/dist/v22-0/arm/disks/ubuntu-18.04-arm64-docker.img.bz2
bunzip2 "${M5_PATH}/disks/ubuntu-18.04-arm64-docker.img.bz2"
```

## Build of bootloaders:

```bash
make -C system/arm/bootloader/arm
make -C system/arm/bootloader/arm64
pushd ${M5_PATH}/binaries
ln -s ../../system/arm/bootloader/arm64/boot.arm64 bootloader
popd
```

## Build of vmlinux:

```bash
# Obtaining resource:
git clone https://gem5.googlesource.com/arm/linux
pushd linux
git checkout gem5/v4.14
# Typical build error pre-removal:
sed -i '634s/YYLTYPE/extern YYLTYPE/' scripts/dtc/dtc-lexer.lex.c_shipped
# Building `vmlinux':
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- gem5_defconfig
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j `nproc`
popd
pushd ${M5_PATH}/binaries
ln -s ../../linux/vmlinux vmlinux
popd
```

## Build of test workload with ARM 3 level cache model:

```bash
# Default:
scons ./build/ARM_MESI_Three_Level/gem5.opt -j$(nproc)
# Optimal, which is without traces and debug info:
scons ./build/ARM_MESI_Three_Level/gem5.fast -j$(nproc)
# Build `m5term' executable:
make -C util/term/
# Working build check:
./build/ARM_MESI_Three_Level/gem5.fast \
--outdir=arm_mesi3level_m5out configs/example/arm/fs_bigLITTLE.py \
--kernel $M5_PATH/binaries/vmlinux \
--bootloader $M5_PATH/binaries/bootloader \
--disk $M5_PATH/disks/ubuntu-18.04-arm64-docker.img \
--root /dev/vda1 \
--caches --mem-size 2GB --last-cache-level 3 \
--machine-type VExpress_GEM5_V1 \
--cpu-type timing \
--dvfs --big-cpus 2 --little-cpus 4 \
--little-cpu-voltage 1.2V 1.1V 1V --little-cpu-clock 1.3GHz 1.2GHz 1GHz \
--big-cpu-voltage 1.2V 1.1V --big-cpu-clock 2GHz 1GHz
# From the another terminal, run:
./util/term/m5term localhost 3456
```

## Compiling of benchmarks:

```bash
# This line creates binaries in the
# arch-se-bench folder
# Now you can add --bootscript flag
# with provided .out test
bash se-benchmarks-compile.sh
```
