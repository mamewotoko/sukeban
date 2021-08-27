#! /bin/bash
set -x

. ./env.sh

echo $UNAME

if [[ "$UNAME" = Darwin ]]; then
    brew update
    brew install gtk+3
    # make gcc pkg-config

elif [[ -f /etc/lsb-release ]]; then
    sudo apt-get update
    sudo apt-get install -y libgtk-3-dev pkg-config \
         xvfb make gcc

elif [[ -f /etc/redhat-release ]] && grep "release 7" /etc/redhat-release; then
    sudo yum install -y gcc gobject-introspection-devel \
         pkg-config gtk3 make

elif [[ "$UNAME" = "MINGW64_NT"* ]]; then
    # msys2 on PC
    # github action
    # TODO; if CI_ENV is not github_action, install make
    pacman -Syu --noconfirm
    pacman -Sy --noconfirm mingw-w64-x86_64-gtk3 \
           mingw-w64-x86_64-gcc \
           mingw-w64-x86_64-pkg-config
    echo /usr/bin
    ls /usr/bin
    echo /usr/local/bin
    ls /usr/local/bin
elif [[ "$UNAME" = "MSYS_NT"* ]]; then
    # travis
    $msys2 pacman -Syu --noconfirm
    $msys2 pacman -Sy --noconfirm mingw-w64-x86_64-gtk3 \
	   mingw-w64-clang-x86_64-gdk-pixbuf2 \
           mingw-w64-x86_64-gcc \
           mingw-w64-x86_64-pkg-config
fi
