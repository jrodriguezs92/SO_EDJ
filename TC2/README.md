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
