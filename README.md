# Touchremin 
An interface made with Arduino, for visuals and music interaction.

This repository has two Arduino versions. One is made for **ATMEGA328P** (Basic Arduino family), with serial communication. This version is able to communicate with the visuals made in *Processing*. The visuals react to the capacitive sensors installed in the interface when they are touched. It uses [AP_Sync](https://ap-sync.github.io/), this library helps the Arduino to send data synchronously to *Processing*, making it easier to understand the information collected by the micro-controller. The *Processing* file also helps to send the Serial data through **OSC** for *PureData*.

The other Arduino version, was made for the Arduino Nano 33 IoT, with **SAMD21G18A**  micro-controller. This code sends the interface data via WiFi using **OSC** (Open Sound Control). It has a header file were it should have the SSID where it's going to be connected and the password.

Finally, there are *PureData* Files that receives the information from an **OSC** message, and creates different sounds. 

You're open to use this code and contribute to it by forking it. 🚀
