#!/usr/bin/env bash
# Fase "prepare" do semantic-release: builda o Map2Check completo (KLEE 3.1 +
# LibFuzzer) dentro da imagem map2check-dev, injetando a versão calculada no
# binário via -DMAP2CHECK_VERSION, e empacota release/ em .zip.
# Chamado pelo @semantic-release/exec: scripts/prepare-release.sh 8.1.0
set -euo pipefail

VERSION="${1:?usage: prepare-release.sh <version>}"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$REPO_ROOT"

docker run --rm \
  -u root \
  -v "$REPO_ROOT:/workspace" \
  -w /workspace \
  -e CC=/usr/bin/clang-16 \
  -e CXX=/usr/bin/clang++-16 \
  -e MAP2CHECK_VERSION="$VERSION" \
  ghcr.io/hbgit/map2check-dev:latest bash -c '
    set -euo pipefail

    # WABT + wasi-sdk (a imagem publicada não os traz)
    if [ ! -f /opt/wabt-1.0.41/include/wasm-rt.h ]; then
      wget -q "https://github.com/WebAssembly/wabt/releases/download/1.0.41/wabt-1.0.41-linux-x64.tar.gz" -O /tmp/wabt.tar.gz
      tar xzf /tmp/wabt.tar.gz -C /opt && rm /tmp/wabt.tar.gz
      ln -sf /opt/wabt-1.0.41/bin/wasm2c /usr/local/bin/wasm2c
    fi
    if [ ! -d /opt/wasi-sdk-33.0-x86_64-linux ]; then
      wget -q "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-33/wasi-sdk-33.0-x86_64-linux.tar.gz" -O /tmp/wasi-sdk.tar.gz
      tar xzf /tmp/wasi-sdk.tar.gz -C /opt && rm /tmp/wasi-sdk.tar.gz
    fi

    rm -rf build release
    mkdir -p build && cd build
    cmake .. -G Ninja \
      -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
      -DCMAKE_INSTALL_PREFIX=/workspace/release \
      -DMAP2CHECK_VERSION="${MAP2CHECK_VERSION}"
    ninja
    ninja install

    # KLEE runtime + clang headers ausentes das regras de install
    # (cp -rL, não symlink: precisa sobreviver dentro do zip)
    mkdir -p /workspace/release/lib/klee
    cp -rL /opt/klee/lib/klee/runtime /workspace/release/lib/klee/runtime
    [ -e /workspace/release/lib/clang ] || \
      cp -rL /usr/lib/llvm-16/lib/clang /workspace/release/lib/clang
  '

# O container roda como root; no CI, devolve a posse ao usuário do runner
if [ -n "${CI:-}" ]; then
  sudo chown -R "$(id -u):$(id -g)" build release
fi

bash scripts/package-release.sh "$VERSION"
