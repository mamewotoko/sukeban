export UNAME="$(uname)"

if [ "$UNAME" = Darwin ]; then
    export PATH="/usr/local/opt/make/libexec/gnubin:$PATH"
elif [[ "$UNAME" == "MSYS_NT"* ]]; then
    export PATH=/mingw64/bin:$PATH
    export LD_LIBRARY_PATH=/mingw64/lib:$LD_LIBRARY_PATH
    export PKG_CONFIG_PATH=/mingw64/lib/pkgconfig
fi     
