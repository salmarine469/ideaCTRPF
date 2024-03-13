# ideaCTRPF

## Usage

### Requirements

```
Git
devkitARM
libctrpf
3gxtool
3ds-mbedtls
3ds-zlib
3ds-liblua51
libncsnd
```

### Clone

```bash
# HTTPS
git clone --recursive https://github.com/salmarine469/ideaCTRPF.git

# SSH
git clone --recursive git@github.com:salmarine469/ideaCTRPF.git
```

### Build

1. Make sure you have [devkitpro](https://devkitpro.org/wiki/Getting_Started) installed and working.
2. Clone or download the repo and open a command prompt.
3. Run `pacman -S libctrpf 3gxtool 3ds-mbedtls 3ds-zlib 3ds-liblua51` to install libraries.
4. Install [libncsnd](https://github.com/PabloMK7/libncsnd).
5. Run `cd ideaCTRPF && make`

### Send

#### 1. Compile plugin-sender-socket/main.cpp

#### 2. Run Update3gx entry in 3ds and select from your pc with socket

#### 3. Run output

```bash
g++ plugin-sender-socket/main.cpp
./a.out {3ds local ip} {3gx path}
```
