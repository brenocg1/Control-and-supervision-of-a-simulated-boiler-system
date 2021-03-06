<!-- Title -->
# Control and supervision of a simulated boiler system

<!-- ABOUT THE PROJECT -->
## About the project

This was my final project for the **real-time systems** discipline. In this project, a program was made to **supervise and control a simulated boiler** through a Java program provided by the book *Fundamentals of Real Time Systems* by **Rômulo Silva de Oliveira**.

<!-- About the program -->
## About the program

The program was done in C, using specific techniques for real-time systems, such as mutual exclusion, monitors and double buffer. The system contains several sensors and actuators, you can see them in more detail and how they were made in the file ***controle-caldeira-2.pdf***, provided by the book. For more information on the implementation, data obtained and on the boiler itself, read the [article](https://github.com/brenocg1/Control-and-supervision-of-a-simulated-boiler-system/blob/master/Controle_e_supervis_o_de_um_sistema_de_caldeira_simulado.pdf).

### Built With
Some things you need to know, or at least, know that exist.

* [C](http://www.open-std.org/jtc1/sc22/wg14/)
* [Makefile](https://www.gnu.org/software/make/manual/make.html#Introduction)
* [Sockets](https://www.geeksforgeeks.org/socket-programming-cc/)

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

* [C compiler](https://gcc.gnu.org/)
* [Boiler simulator provided by the book]
* [JRE](https://www.java.com/pt_BR/download/)

<!-- Some results -->
## Some results
#### Height variation in test 1
![height in test 1](plots/t1_altura.png)

#### Temperature variation in test 1
![temp in test 1](plots/t1_temp.png)

#### Height variation in test 2
![height in test 2](plots/t2_altura.png)

#### Temperature variation in test 2
![temp in test 2](plots/t2_temp.png)

#### Height and flow of water through the controlled tap in test 2
![Height and flow of water](plots/t2_altura_No.png)

#### Response time of the thread that controls the height
![thread response time](plots/tempResp_altura.png)
