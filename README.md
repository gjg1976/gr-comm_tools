# gr-comm_tools
The gr-comm_tools module implements a BPSK and QPSK phase ambiguity solvers based on a sync word detection. 

Three blocks are included, a BPSK phase ambiguity resolver for data streams, a QPSK phase ambiguity resolver for data streams and a BPSK/QPSK phase ambiguity resolver for messages

In the docs folder there is a pdf explained the theory behind using standard blocks, the hier blocks from the pdf are included in the examples folder

Installation
To install comm_tools, you should clone the repository and:

mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig

The module can be uninstalled using:

cd build
sudo make uninstall
sudo ldconfig
