FROM ubuntu:18.04

RUN apt-get update && apt-get install -y curl libicu60 libusb-1.0-0 libcurl3-gnutls git cmake g++ nodejs npm nano

# Install EOSIO
RUN curl -LO https://github.com/EOSIO/eos/releases/download/v1.8.9/eosio_1.8.9-1-ubuntu-18.04_amd64.deb \
    && dpkg -i eosio_1.8.9-1-ubuntu-18.04_amd64.deb

# Download and unpackage EOSIO.CDT 1.4.1
# RUN curl -o /eosio.cdt/eosio.cdt-1.4.1.x86_64.deb --create-dirs -L https://github.com/EOSIO/eosio.cdt/releases/download/v1.4.1/eosio.cdt-1.4.1.x86_64.deb \
#    && dpkg-deb -x /eosio.cdt/eosio.cdt-1.4.1.x86_64.deb /eosio.cdt/v1.4.1

# Download and unpackage EOSIO.CDT 1.5.0
 RUN curl -o /eosio.cdt/eosio.cdt_1.5.0-1_amd64.deb --create-dirs -L https://github.com/EOSIO/eosio.cdt/releases/download/v1.5.0/eosio.cdt_1.5.0-1_amd64.deb \
    && dpkg-deb -x /eosio.cdt/eosio.cdt_1.5.0-1_amd64.deb /eosio.cdt/v1.5.0

# Download and unpackage EOSIO.CDT 1.6.3
RUN curl -o /eosio.cdt/eosio.cdt_1.6.3-1-ubuntu-18.04_amd64.deb --create-dirs -L https://github.com/EOSIO/eosio.cdt/releases/download/v1.6.3/eosio.cdt_1.6.3-1-ubuntu-18.04_amd64.deb \
    && dpkg-deb -x /eosio.cdt/eosio.cdt_1.6.3-1-ubuntu-18.04_amd64.deb /eosio.cdt/v1.6.3

# Download and unpackage EOSIO.CDT 1.7.0
RUN curl -o /eosio.cdt/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb --create-dirs -L https://github.com/EOSIO/eosio.cdt/releases/download/v1.7.0/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb \
    && dpkg-deb -x /eosio.cdt/eosio.cdt_1.7.0-1-ubuntu-18.04_amd64.deb /eosio.cdt/v1.7.0

# Download EOSIO.Contracts
RUN curl -LO https://github.com/EOSIO/eosio.contracts/archive/v1.8.3.tar.gz && tar -xzvf v1.8.3.tar.gz --one-top-level=eosio.contracts --strip-components 1

# Activate EOSIO.CDT 1.6.3
RUN cp -rf /eosio.cdt/v1.6.3/usr/* /usr/

# Build EOSIO.Contracts
RUN cd /eosio.contracts/ && mkdir build && cd build && cmake .. && make all

# Download ORE.Contracts
# RUN git clone https://github.com/API-market/eosio.contracts /ore.contracts/

# Build ORE.Contracts
# RUN cd /ore.contracts/ && git checkout ore && mkdir -p build && rm -rf build/* && sed -i "s/make\s-j\s\$CPU_CORES/make -j \$CPU_CORES contracts_project/" build.sh && ./build.sh -e /usr/opt/eosio/1.8.9/ -c /eosio.cdt/v1.6.3/usr/
