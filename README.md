# Progetto Sistemi Operativi

### Cpu Scheduling Simulator:
The objective of this small project is to simulate the behaviour of a OS managing the CPU scheduling. Modified to handle multiple (configurable) cpus and preemptive shortest job first with quantum prediction ``` q(t+1) = a * q_current + (1-a) * q(t) ```.


OS and processes are represented by ```fake_os``` and ```fake_process```.

Some ```.txt``` files are also present for testing.
