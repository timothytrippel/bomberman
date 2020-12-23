FROM ubuntu:18.04
MAINTAINER trippel@umich.edu
ENV TERM xterm-256color
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get upgrade -y && apt-get autoremove -y
 
# Setup directory structure
ENV SRC=/src
ENV BOMBERMAN=$SRC/bomberman
ENV PATH="$PATH:/scripts"
RUN mkdir -p $SRC $BOMBERMAN && chmod a+rwx $SRC $BOMBERMAN

# Install packages
# TODO(ttrippel): install specific versions of tools
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

# Build/Install DFG Generator
RUN cd $BOMBERMAN/tgt-ttb && make

# Set entrypoint
WORKDIR $BOMBERMAN
COPY run.sh $BOMBERMAN/
CMD ["/bin/bash"]
