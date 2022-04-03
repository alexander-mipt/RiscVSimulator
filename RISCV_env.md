# Установка gnu-toolchain
```
export RISCV32=/opt/riscv32/
sudo ./configure --prefix=$RISCV32 --with-arch=rv32i
sudo make
```

in `config.log`:
```
WITH_ABI='--with-abi=ilp32'
WITH_ARCH='--with-arch=rv32i'

default_target='newlib'
```
