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
            buildInputs = [ esp-idf-full ];

            # shellHook = ''
            #   source ${esp-idf-full}/export.sh
            # '';
          };

      });
}
