# Description
boot.asm -> HELLO WORLD FROM 16 BIT REAL MODE 

boot2.asm -> HELLO WORLD FROM 32 BIT PROTECTED MODE

boot3.asm -> HELLO WORLD FROM 32 BIT PROTECTED MODE LOADING 512 BYTES FROM SECOND SECTOR INTO MEMORY

# Getting Start  
Install nasm and qemu
```
sudo apt-get install nasm qemu
```

# Instructions

## Compile  
```
nasm boot.asm -f bin -o boot.bin
```

## Execute
```
qemu-system-i386 -fda boot.bin
```

## Generate binary for USB boot
```
nasm -f bin boot.asm -o boot.com
```

## Get path for USB drive (usbPath)
```
sudo fdisk -l
```

## Copy in USB drive
```
sudo dd if=boot.com of=<usbPath> bs=512 count=1
```
