#Introduction
SCTest is a unit test framework for SystemC.
Unit tests for SystemC have some differences from normal software unit tests. SystemC describes hardware, and the simulation is run in a simulation engine. So test framworks like gtest is not quite sutable for SystemC. I think unit tests for a SystemC module is similar to tests a real chip. You put a chip on a testbench, give some inputs and compare the output to a golden pattern.

#Example
A simple 

