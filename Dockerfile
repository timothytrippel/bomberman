# Copyright © 2020, Timothy Trippel <trippel@umich.edu>
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

FROM ubuntu:18.04
MAINTAINER trippel@umich.edu
ENV TERM xterm-256color
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get upgrade -y && apt-get autoremove -y
 
# Setup directory structure
ENV SRC=/src
ENV BOMBERMAN=$SRC/bomberman
ENV PATH="$PATH:$BOMBERMAN"
RUN mkdir -p $SRC $BOMBERMAN && chmod a+rwx $SRC $BOMBERMAN

# Install packages
# TODO(timothytrippel): install specific versions of tools
RUN apt-get install -y \
    git \
    build-essential \
    binutils \
    bison \
    flex \
    autoconf \
    gperf \
    graphviz \
    python3

# Build/Install Icarus Verilog (version 10.3)
COPY iverilog_main.patch $BOMBERMAN/
RUN cd $BOMBERMAN && \
      git clone https://github.com/steveicarus/iverilog.git && \
      cd iverilog && \
      git checkout 453c5465895eaca4a792d18b75e9ec14db6ea50e && \
      mv ../iverilog_main.patch ./ && \
      patch <iverilog_main.patch && \
      sh autoconf.sh && \
      ./configure --prefix=$(pwd) && \
      make -j$(nproc) install
ENV PATH="$PATH:$BOMBERMAN/iverilog/bin"

# Build/Install RISC-V Toolchain
RUN apt-get install -y \
    automake \
    autotools-dev \
    curl \
    libmpc-dev \
    libmpfr-dev \
    libgmp-dev \
    gawk \
    texinfo \
    libtool \
    patchutils \
    bc \
    zlib1g-dev \
    libexpat1-dev
RUN mkdir /opt/riscv32i
RUN cd $SRC && git clone https://github.com/riscv/riscv-gnu-toolchain \
      riscv-gnu-toolchain-rv32i && \
      cd riscv-gnu-toolchain-rv32i && \
      git checkout 411d134 && \
      git submodule update --init --recursive
RUN cd $SRC/riscv-gnu-toolchain-rv32i && mkdir build && cd build && \
      ../configure --with-arch=rv32i --prefix=/opt/riscv32i && \
      make -j$(nproc)

# Set entrypoint
COPY analyze_all.sh $BOMBERMAN/
WORKDIR $BOMBERMAN
CMD ["/bin/bash"]
