#!/bin/sh

bin/build-emcpp.sh &&
cp build/MyMoneroLibAppCpp_WASM.js libapp_js/; 
cp build/MyMoneroLibAppCpp_WASM.wasm libapp_js/;