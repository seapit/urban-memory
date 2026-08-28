# syntax=docker/dockerfile:1

# Specify the variant
ARG VARIANT=ubuntu-24.04
FROM mcr.microsoft.com/devcontainers/cpp:1-${VARIANT}

RUN rm -f /etc/apt/apt.conf.d/docker-clean \
  && echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' \
     > /etc/apt/apt.conf.d/keep-cache

# [Optional] Uncomment this section to install additional packages.
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
  apt-get update && export DEBIAN_FRONTEND=noninteractive \
  && apt-get -y install --no-install-recommends \
  ccache \
  curl \
  clangd \
  clang-tools-18 \
  default-jdk\
  flawfinder \
  graphviz \
  iwyu \
  jq \
  pipx \
  python3.12-venv \
  shellcheck \
  shfmt \
  syslog-ng \
  syslog-ng-core \
  unzip

ARG GTEST_TAG=latest
RUN set -eu; \
  if [ "$GTEST_TAG" = "latest" ]; then \
    GTEST_TAG="$(git ls-remote --tags --refs --sort=-v:refname \
      https://github.com/google/googletest.git 'v*' \
      | sed 's|.*refs/tags/||' \
      | grep -Ev -- '-(pre|rc|alpha|beta)' \
      | head -n1)"; \
    [ -n "$GTEST_TAG" ] || { echo "could not resolve a googletest tag" >&2; exit 1; }; \
  fi; \
  echo "building googletest ${GTEST_TAG}"; \
  git clone --depth 1 -b "${GTEST_TAG}" \
    https://github.com/google/googletest.git /googletest; \
  cmake -S /googletest -B /googletest/build -DCMAKE_BUILD_TYPE=Release; \
  cmake --build /googletest/build -j"$(nproc)"; \
  cmake --install /googletest/build

# ARCHIVED APT-GETS
# # Install Clang-format, gtest, wget, python, pipxs
# && apt-get -y install --no-install-recommends binutils g++ gcc gcovr \
# libgtest-dev pipx python3.12 shfmt syslog-ng syslog-ng-core wget  \
# && apt-get clean && rm -rf /var/lib/apt/lists/*

ARG USERNAME=vscode
# Switch to vscode user to install other items I need.
USER vscode

RUN   pipx ensurepath \
  && pipx install semgrep \
  && pipx install cmake-format --include-deps \
  && pipx install clang-format --include-deps