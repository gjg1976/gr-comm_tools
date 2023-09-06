# gr-comm_tools
The gr-comm_tools module implements a BPSK and QPSK phase ambiguity solvers based on a sync word detection. 

Three blocks are included, a BPSK phase ambiguity resolver for data streams, a QPSK phase ambiguity resolver for data streams and a BPSK/QPSK phase ambiguity resolver for messages

In the docs folder there is a pdf explained the theory behind using standard blocks, the hier blocks from the pdf are included in the examples folder

## Installation
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

## QPSK Phase Ambiguity Solver
This block will solves the 90º phase rotation and invertion ambiguity in a QPSK demodulated signal when the signal has ASM

The input signal is the demodulated bit stream with phase ambiguity and the output is the same signal but with the ambiguity solved 
### Parameters
(R): Run-time adjustable
#### Error Tolerance (R)
Set the number of bit than can be “skipped” from the ASM correlation as ASM bit error ignoring
#### Syncword (R)
Set syncword to look for Phase Ambiguity resolution as hex string, the hex string could have an odd number of characters 
#### Lock Msg
Set the command message send through the message port when ASM is detected

### Messages
#### LOCK
The pair (Lock Msg, phase value) message sent when the ASM is detected. Phase value integer could be 0, 90, 180, 270, -0, -90, -180, -270

### Example Flowgraph
This example flowgraph qpsk_phase_ambiguity_solver_test_v1.grc is located in the gr-com_tools package

![Alt text](./images/qpsk_ambiguity_solver.png?raw=true "QPSK Phase Ambiguity Solver Example")
   
In this example, the bytes of and input files are convolutional codec and QPSK modulated. The modulated signal is rotated 0º, 90º, 180º or 270º and each of these rotation are I-Q inverted. A Selector allows to choose with signal is demodulated. At the QPSK demodulator output, a QPSK Phase ambiguity solver will resolve the phase ambiguity and the “fixed” signal is decode and the the PDUs when Syncmark start are generated and stored in an output file.

## BPSK Phase Ambiguity Solver
This block will solves the 180º phase ambiguity in a BPSK demodulated signal when the signal has a sync mark (ASM)

The input signal is the demodulated bit stream with phase ambiguity and the output is the same signal but with the ambiguity solved 
### Parameters
(R): Run-time adjustable
#### Error Tolerance (R)
Set the number of bit than can be “skipped” from the ASM correlation as ASM bit error ignoring
#### Syncword (R)
Set syncword to look for Phase Ambiguity resolution as hex string 
#### Lock Msg
Set the command message send through the message port when ASM is detected

### Messages
#### LOCK
The pair (Lock Msg, phase value) message sent when the ASM is detected. Phase value integer could be 0 or 180
### Example Flowgraph
This example flowgraph bpsk_phase_ambiguity_solver_test_v1.grc is located in the gr-com_tools package
![Alt text](./images/bpsk_ambiguity_solver.png?raw=true "BPSK Phase Ambiguity Solver Example")

In this example, the bytes of and input files are BPSK modulated. The modulated signal is rotated 0º, or 180º. A Selector allows to choose with signal is demodulated. At the BPSK demodulator output, a BPSK Phase ambiguity solver will resolve the phase ambiguity and the “fixed” signal is decode and the the PDUs when Syncmark start are generated and stored in an output file.

## PDU Phase Ambiguity Solver
This block is based on the QPSK and BPSK Ambiguity solvers for streams but for PDUs. It takes a constellation QPSK or BPSK output data bytes and fix the phase ambiguity using ASM when constellation modulation is applied  will solves the 180º phase ambiguity in a BPSK demodulated signal when the signal has a sync mark (ASM)

The input signal is the demodulated bits message with phase ambiguity and the output is the same bit message but with the ambiguity solved 
### Parameters
(R): Run-time adjustable
#### Modulation
Set if the input data is from a BPSK or QPSK demodulator
#### Error Tolerance (R)
Set the number of bit than can be “skipped” from the ASM correlation as ASM bit error ignoring
#### Syncword (R)
Set syncword to look for Phase Ambiguity resolution as hex string 
#### Lock Msg
Set the command message send through the message port when ASM is detected

### Messages
#### LOCK
The pair (Lock Msg, phase value) message sent when the ASM is detected. Phase value integer could be 0 or 180
### Example Flowgraph
This example flowgraph pdu_phase_ambiguity_solver_test_v1.grc is located in the gr-com_tools package

![Alt text](./images/pdu_ambiguity_solver.png?raw=true "PDU Phase Ambiguity Solver Example")

In this example, a number of vector sources is used to generated demodulated BPSK and QPSK data with the ASM in all the phase rotation, the PDU Phase ambiguity solver will resolve the phase ambiguity and the output data shall match the expected one.
