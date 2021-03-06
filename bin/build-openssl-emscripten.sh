#!/bin/sh

PLATFORM="emscripten"

SRC_DIR="contrib/openssl"
INSTALL_DIR="build/openssl"

SRC_PATH="$(pwd)/$SRC_DIR"
INSTALL_PATH="$(pwd)/$INSTALL_DIR"

if [ ! -d "$SRC_PATH" ]; then
  echo "SOURCE NOT FOUND!"
  exit 1
fi

# ---

cd "$SRC_PATH"

#linux-generic32 \

emconfigure ./Configure \
	linux-generic32 \
	-no-asm no-ssl2 no-ssl3 no-comp no-engine no-deprecated shared no-dso \
	--prefix="$INSTALL_PATH" \
	--openssldir="$INSTALL_PATH" \
	--cross-compile-prefix=" " \
	2>&1

if [ $? != 0 ]; then
  echo "ERROR: OpenSSL Configure FAILED!"
  exit 1
fi

to_defined_atomics_line='\&\& !defined(__STDC_NO_ATOMICS__) \&\& !defined(__EMSCRIPTEN__)'
sed -i.bak 's/\&\&\ !defined(__STDC_NO_ATOMICS__)$/'"$to_defined_atomics_line"'/' include/internal/refcount.h #the pattern is relying here on the fact the "ATOMICS__)" comes at the end of the line
# ^-- not 'g' b/c we only expect one


# ---
# Clean 
rm -rf "$INSTALL_PATH"
mkdir "$INSTALL_PATH"

emmake make \
  2>&1


if [ $? != 0 ]; then
  echo "ERROR: emmake OpenSSL FAILED!"
  exit 1
fi

# now we must move build products by manually calling make install

make install \
	2>&1

