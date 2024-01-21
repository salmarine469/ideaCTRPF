# ideaCTRPF

## Usage

### Requirements

```
Git
devkitARM
libctrpf
3gxtool
3ds-curl
3ds-mbedtls
3ds-zlib
3ds-liblua51
[libncsnd](https://github.com/PabloMK7/libncsnd)
Python (for sending)
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
3. Run `pacman -S libctrpf 3gxtool 3ds-curl 3ds-mbedtls 3ds-zlib 3ds-liblua51` to install libraries.
4. Install [libncsnd](https://github.com/PabloMK7/libncsnd).
5. Run `cd ideaCTRPF && make`

### Send

#### 1. To install the dependencies, run

```bash
pip install python-dotenv
```

#### 2. Create a `.env` file in the `plugin-sender` directory of the project and add settings according to the following
```
# Plugin file name to send
PLUGIN_NAME=ideaCTRPF.3gx

# Target title id
TITLE_ID=0004000000155100

# 3ds hostname
HOSTNAME=192.168.2.10

# Send type (Optional): 3ds(default), citra, both
SEND_TO=3ds

# Send plugin as default.3gx (Optional): true, false(default)
AS_DEFAULT=false

# sdmc path of citra (Optional)
CITRA_SDMC_PATH=/home/user/.var/app/org.citra_emu.citra/data/citra-emu/sdmc/
```

#### 3. Run the script

```bash
make send
```
