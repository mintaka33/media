# Linux Media Driver 

## Build instructions

### 1. Build libva

```bash
cd libva

./autogen.sh CFLAGS=-g CXXFLAGS=-g

make

sudo make install
```

### 2. Build driver

```bash
mkdir build

cd build

cmake ../media-driver \
-DCMAKE_INSTALL_PREFIX=/usr \
-DMEDIA_VERSION="2.0.0" \
-DBUILD_ALONG_WITH_CMRTLIB=1 \
-DBS_DIR_GMMLIB=`pwd`/../gmmlib/Source/GmmLib/ \
-DBS_DIR_COMMON=`pwd`/../gmmlib/Source/Common/ \
-DBS_DIR_INC=`pwd`/../gmmlib/Source/inc/ \
-DBS_DIR_MEDIA=`pwd`/../media-driver

make -j8

sudo make install
```
note: if build debug version driver, add below line in cmake command
```bash
-DBUILD_TYPE=Debug
```

### 3. Build libva-utils

```bash
cd libva-utils

./autogen.sh CFLAGS=-g CXXFLAGS=-g

make

sudo make install
```

## Run driver

### 1. Set environment variables
* Edit .bashrc file
```bash
cd ~
nano .bashrc
```
* Add below lines at the bottom of .bashrc file
```
export LD_LIBRARY_PATH=/usr/local/lib
export LIBVA_DRIVERS_PATH=/usr/local/lib/dri
export LIBVA_DRIVER_NAME=iHD
```
* Restart terminal

### 2. Run libva-utils sample applications

  * Run vainfo to query driver capabilities
```bash
cd libva-utils/vainfo

./vainfo
```
  * Run decode test
```bash
cd libva-utils/decode

./mpeg2vldemo
```

  * Run encode test
```bash
cd libva-utils/encode

./avcenc
```

## Reference

### 1. libva
- https://github.com/01org/libva/commit/df544cd5a31e54d4cbd33a391795a8747ddaf789
### 2. media-driver
- https://github.com/intel/media-driver/commit/5c37e4fc6dcdd392fff7945c7aea775f475f7ee6
### 3. gmmlib
- https://github.com/intel/gmmlib/commit/b1451bbe4c506f17ddc819f12b4b448fc08698c5
### 4. libva-utils
- https://github.com/01org/libva-utils/commit/7b85ff442d99c233fb901a6fe3407d5308971645
