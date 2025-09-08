# Copyright 2025 M. MAD

{ pkgs ? import <nixpkgs> { } }:

with pkgs; let
  python = [ python312Full ];
in mkShell {
  buildInputs = with python312Packages; python ++ [
    ipython

    # General
    ipython
    jupyter
    jupyterlab
    jupyterlab-git
    jupyterlab-lsp
    ipylab  # https://github.com/jtpio/ipylab
    attrs
    future-typing  # https://github.com/PrettyWood/future-typing

    # blogger.py
    jinja2
    types-jinja2
    qrcode
    # `qrcode` dependencies
    pillow
    types-pillow

    # museum_website
    beautifulsoup4  # For index generator
    types-beautifulsoup4
    python-frontmatter  # Also for index generator
  ];

  shellHook = ''
    # ...
  '';
}
