let
    pkgs = import <nixpkgs> {};
    cross = pkgs.pkgsCross.x86_64-embedded;
in
pkgs.mkShell {
    nativeBuildInputs = with pkgs.buildPackages; [
        automake
        autoconf
        cmake
        gdb
        nasm
        parted
        cross.buildPackages.binutils
        cross.buildPackages.gcc
    ];
}