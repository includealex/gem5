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

## Disk modification
Firstly, you'll need an ability to change image files:
```bash
cd $M5_PATH/disks/
mkdir -p mnt/
# ubuntu-18.04-arm64-docker.img was already installed
../../util/gem5img.py mount ubuntu-18.04-arm64-docker.img mnt
sudo cp /etc/resolv.conf mnt/etc/
```

- Serial terminal creation. Create the `mnt/etc/init` directory. Then Modify the `mnt/etc/init/tty-gem5.conf` file:
```
# ttyS0 - getty
#
# This service maintains a getty on ttyS0 from the point the system is
# started until it is shut down again, unless there is a script passed to gem5.
# If there is a script, the script is executed then simulation is stopped.

start on stopped rc RUNLEVEL=[12345]
stop on runlevel [!12345]

console owner
respawn
script
   # Create the serial tty if it doesn't already exist
   if [ ! -c /dev/ttyS0 ]
   then
      mknod /dev/ttyS0 -m 660 /dev/ttyS0 c 4 64
   fi

   # Try to read in the script from the host system
   /sbin/m5 readfile > /tmp/script
   chmod 755 /tmp/script
   if [ -s /tmp/script ]
   then
      # If there is a script, execute the script and then exit the simulation
      exec su root -c '/tmp/script' # gives script full privileges as root user in multi-user mode
      /sbin/m5 exit
   else
      # If there is no script, login the root user and drop to a console
      # Use m5term to connect to this console
      exec /sbin/getty --autologin root -8 38400 ttyS0
   fi
end script
```

- Localhost setup. Put this into `mnt/etc/hosts` file:
```
127.0.0.1 localhost
::1 localhost ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
ff02::3 ip6-allhosts
```

- Fstab setup. Put this into `mnt/etc/fstab` file :
```
# /etc/fstab: static file system information.
#
# Use 'blkid' to print the universally unique identifier for a
# device; this may be used with UUID= as a more robust way to name devices
# that works even if disks are added and removed. See fstab(5).
#
# <file system>    <mount point>   <type>  <options>   <dump>  <pass>
/dev/hda1      /       ext3        noatime     0 1
```

- Exit:
```bash
../../util/gem5img.py umount mnt
```

## Compiling of Stanford benchmarks:

```bash
# This line creates binaries in the
# arch-se-bench folder
# Now you can add --bootscript flag
# with provided .out test
bash se-benchmarks-compile.sh
```
