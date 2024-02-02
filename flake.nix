{
  description = "Kakoune with tree-sitter hightlighting";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = {
    self,
    nixpkgs,
    flake-parts,
  } @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux" "aarch64-linux"];
      perSystem = {
        lib,
        pkgs,
        self',
        ...
      }: {
        formatter = pkgs.alejandra;

        devShells.default = pkgs.mkShell {
          inputsFrom = [self'.packages.default];
          packages = with pkgs; [clang-tools];
          NIX_HARDENING_ENABLE = "";
          MAKEFLAGS = "debug=yes -j8";
        };

        packages.default = pkgs.callPackage ({
          tree-sitter,
          tree-sitter-grammars,
          clangStdenv,
          pkg-config,
          lib,
          src,
        }:
          clangStdenv.mkDerivation {
            pname = "kakoune";
            version = "2023.08.05";
            inherit src;
            nativeBuildInputs = [pkg-config];
            buildInputs = [tree-sitter];
            makeFlags = ["debug=no" "PREFIX=${placeholder "out"}"];
            enableParallelBuilding = true;
            meta = with lib; {
              description = "Modal editor - Faster as in fewer keystrokes - Multiple selections - Orthogonal design";
              homepage = "https://kakoune.org";
              mainProgram = "kak";
              license = licenses.unlicense;
              platforms = platforms.unix;
            };
          }) {src = self;};
      };
    };
}
