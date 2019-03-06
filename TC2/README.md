## Getting Start  
Install nasm and qemu
```
sudo apt-get install nasm qemu
```

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

## Copy in USB 
```
sudo dd if=boot.com of=/dev/sdb bs=512 count=1
```
