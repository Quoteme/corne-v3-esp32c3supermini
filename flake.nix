{
  description = "My development shell for all my RV projects";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils = {
      inputs.nixpkgs.follows = "nixpkgs";
      url = "github:numtide/flake-utils";
    };
    esp32.url = "github:mirrexagon/nixpkgs-esp-dev";
  };

  outputs = { nixpkgs, flake-utils, esp32, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ (import "${esp32}/overlay.nix") ];
        };
      in rec {
        devShells.default = with pkgs;
          mkShell {
            buildInputs = [
              esp-idf-full
              cmake-language-server
              cmake-format
              cmake-lint
              (pkgs.writeShellScriptBin "debug-tui" # bash
                ''
                  #!/usr/bin/env bash
                  idf.py openocd
                  # https://github.com/kovidgoyal/kitty/issues/1613#issuecomment-2328139105
                  TERM="xterm-256color" idf.py gdbtui
                '')
            ];

            OPENOCD_SCRIPTS = "${pkgs.openocd}/share/openocd/scripts";
            ESP_ROM_ELF_DIR = "${pkgs.openocd}";

            # shellHook = ''
            #   source ${esp-idf-full}/export.sh
            # '';
          };

      });
}
