################################################################################
# quartus image

FROM ubuntu:20.04 as fpga_install

ARG QUARTUS_MAJOR_VERSION=23.1
ARG QUARTUS_MINOR_VERSION=1
ARG QUARTUS_REVISION_VERSION=993
ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /tmp

ARG INTEL_CDN="https://downloads.intel.com/akdlm/software/acdsinst"

ADD ${INTEL_CDN}/${QUARTUS_MAJOR_VERSION}std/${QUARTUS_REVISION_VERSION}/ib_installers/QuartusLiteSetup-${QUARTUS_MAJOR_VERSION}std.${QUARTUS_MINOR_VERSION}.${QUARTUS_REVISION_VERSION}-linux.run .
ADD ${INTEL_CDN}/${QUARTUS_MAJOR_VERSION}std/${QUARTUS_REVISION_VERSION}/ib_installers/cyclonev-${QUARTUS_MAJOR_VERSION}std.${QUARTUS_MINOR_VERSION}.${QUARTUS_REVISION_VERSION}.qdz .

RUN chmod a+x QuartusLiteSetup-${QUARTUS_MAJOR_VERSION}std.${QUARTUS_MINOR_VERSION}.${QUARTUS_REVISION_VERSION}-linux.run && \
    ./QuartusLiteSetup-${QUARTUS_MAJOR_VERSION}std.${QUARTUS_MINOR_VERSION}.${QUARTUS_REVISION_VERSION}-linux.run --mode unattended --accept_eula 1 --installdir /opt/intelFPGA && \
    rm -rf /opt/intelFPGA/uninstall/

################################################################################
# riscv-gnu-toolchain image

FROM ubuntu:20.04 as riscv_install

ARG RISCV_TOOLCHAIN_VERSION=2024.09.03
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y --no-install-recommends autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev ca-certificates && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* 

WORKDIR /tmp/

RUN  git clone --branch ${RISCV_TOOLCHAIN_VERSION} https://github.com/riscv/riscv-gnu-toolchain

WORKDIR /tmp/riscv-gnu-toolchain

RUN ./configure --prefix=/opt/riscv --with-arch=rv32emc --with-abi=ilp32e
RUN make -j $(nproc)

################################################################################
# ossc_pro build image

FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386
RUN apt-get update && \
    apt-get upgrade -y && \ 
    apt-get install -y --no-install-recommends libglib2.0-0 libxext6 libmpc3 libfontconfig1 locales patch build-essential &&\ 
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8

ENV QUARTUS_PATH=/opt/intelFPGA
ENV QUARTUS_ROOTDIR=${QUARTUS_PATH}/quartus
ENV SOPC_KIT_NIOS2=${QUARTUS_PATH}/nios2eds
ENV PATH=${QUARTUS_ROOTDIR}/bin/:${QUARTUS_ROOTDIR}/linux64/gnu/:${QUARTUS_ROOTDIR}/sopc_builder/bin/:/opt/riscv/bin:$PATH

COPY --from=fpga_install /opt/intelFPGA/ /opt/intelFPGA/
COPY --from=riscv_install /opt/riscv /opt/riscv

VOLUME  /build
WORKDIR /build