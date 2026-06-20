you can run the solver using the provided makefile:

make

make run ARGS="data.txt"

data.txt (or any other txt file) provides the circuit netlist formatted as follows:

TNNN N1 N2 V

where:
T: a single character corresponding to component type
V - voltage source
I - current source
R - resistance
L - inductance
C - capacitance

NNN: unique component name, from 1 to 20 characters in length
N1, N2: node names where N1 is higher voltage for sources, from 1 to 20 characters in length
V: element value, max 30 characters in length

example values:
10k
-15.2u
50
