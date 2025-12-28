#!/usr/bin/env bash

set -e

update MSYS2 system
pacman -Syu -y

# install build tools + portaudio
pacman -S --needed -y \
    base-devel -y \
	mingw-w64-ucrt-x86_64-toolchain -y \
	mingw-w64-ucrt-x86_64-portaudio -y

# vim config path
vim_setting="$HOME/.vimrc"

# create .vimrc if not exists
if [ ! -f "$vim_setting" ]; then
    cat << 'EOF' > "$vim_setting"
    set number
    set tabstop=4
    set shiftwidth=4
    set expandtab
    set autoindent
    syntax on
EOF
fi
