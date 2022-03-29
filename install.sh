#!/bin/bash
set -eu -o pipefail

sudo -n true

if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

echo "Start installing dependencies"

bin_dir="bin/"

[ ! -d "$bin_dir" ] && mkdir -p "$bin_dir"

while read -r p ; do sudo apt-get install -y $p ; done < <(cat << "EOF"
    cmake
    extra-cmake-modules
    curl
    libcurl3-dev
    libjsoncpp-dev
    libcurl4-gnutls-dev
    libcurl4-openssl-dev
    libcurl4-nss-dev
    libwebsockets-dev
    libboost-all-dev
    libmysqlcppconn-dev
    libmysqlclient-dev
    libmysql++-dev
    mysql-server
    python3-dev
EOF
)

ln -s /usr/include/jsoncpp/json/ /usr/include/json

systemctl start mysql

pip3 install -r requirements.txt

cd ./depends/cxxopts
cmake .
make -j8
make install

cd ./depends/RandomX
cmake .
make -j8 
make install
