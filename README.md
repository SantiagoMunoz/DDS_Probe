# DDS Probe

This is a WIP application that reports the contents of a DDS network. As for now, it is not finished. Im am working on it on my spare time.

## Current Status

This application is unfinished, although there is not much left to do. Expect a working version soon.

## How to install it

You will need CMake to compile this tool. Since it relies of Fast RTPS, you can instruct CMake to build it too

    $ mkdir build; cd build; cmake ../ -DTHIRDPARTY=ON
    $ make

If you already have Fast RTPS installed in your system, you can skip it by not specifying the THIRDPARTY argument.

## How to use it

To run the probe, simply execute the program specifying the target Domain ID you want to scan.

    $ dds_probe 80 //Scans Domain ID 80

The program takes 3 seconds to make sure the Discovery Process of DDS is completed succesfully and then returns a list of present topic names and types on the network.
If you prefer, you can redirect the ouput to a file:

    $ dds_probe 80 > output.txt 2>&1

## Future plans

I am doing this because to speed up things at work, so I am implementing what I need the most. If you would like to see more functions in this, open an issue and I will be happy to take a look at it.
