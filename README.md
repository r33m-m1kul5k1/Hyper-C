# Hyper-C

a native hypervisor for [CrabOS](https://github.com/r33m-m1kul5k1/CrabOS) and Windows.

## Build & Run

```bash
./scripts/run.sh
```

### debug

```bash
mkdir ~/.config/gdb/ && echo "set auto-load safe-path /" > ~/.config/gdb/gdbinit
./scripts/debug.sh

gdb <the file you want to debug>
```

## Bochs configuration

```bash
./configure --enable-x86-64 \
--enable-vmx \
--enable-gdb-stub \
--enable-iodebug \
--enable-long-phy-address
```
--enable-debugger and --enable-gdb-stub are mutually exclusive
## Resources

- [intel's developer manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [osdev](https://wiki.osdev.org)

Tutorials

- [intermezzOS](https://intermezzos.github.io/book/first-edition/jumping-headlong-into-long-mode.html)
- [hypervisor from scratch (windows)](https://rayanfam.com/topics/hypervisor-from-scratch-part-1/)
- [hypervisor from scratch (linux)](https://nixhacker.com/developing-hypervisior-from-scratch-part-1/)
- native hypervisor from scratch [part 1](https://www.digitalwhisper.co.il/files/Zines/0x7C/DW124-1-NativeHyperVisoer.pdf) & [part 2](https://www.digitalwhisper.co.il/files/Zines/0x7D/DW125-1-NativeHyperVisoer-Part2.pdf)

Projects

- [rust-os](https://github.com/nikofil/rust-os)
- [SimpleVisor](https://github.com/ionescu007/SimpleVisor)
- [HyperWin](https://github.com/amiryeshurun/HyperWin)

for more resources [click me](https://github.com/Wenzel/awesome-virtualization)
