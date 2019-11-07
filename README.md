<img align="right" width="200" height="200" src="/images/logo.png">

# GeoNS

## Geometric Large Scale Wireless Network Simulator

GeoNS is an open-source platform to simulate large-scale wireless networks. Contrary to the existing network simulators, which are location-transparent in their indexing, GeoNS uses location-dependent indexing in storing nodes of the network. In other words, we propose to view a wireless network as a spatial database, which allows us to use highly sophisticated spatial data structures in storing nodes of the network.

* Run experiments in serial mode (helpful for debugging during development, or maybe sufficient for experiments).
* Run experiments fully parallelized.
* Synchronous or asynchronous reinforcement learning.
* Reinforcement learning is implemented with the help of *RLLib* (see https://github.com/HerveFrezza-Buet/RLlib).
* Modularity for easy modification / re-use of existing components.

### Getting Started
* Download
Clone this repository to the local machine. You can use the git to clone or get the project via downloading the zip file.
```console
$ git clone https://github.com/roamiri/GeoNS.git
```
* Requirements
Follow the instructions below to install the required packages to build GeoNS. (The platform is tested on LINUX Fedora 18, 30, and 31.)

```console
$ dnf groupinstall 'C Development Tools and Libraries'
$ dnf install cmake boost boost-devel gsl gsl-devel python2 python2-devel numpy python2-matplotlib
```
* Compilation
Enter the GeoNS folder and follow the instructions below. 
```console
$ mkdir build
$ cd build
$ cmake ../CMakeLists.txt
$ make
```
### Implemented Algorithms

There are three applications implemented in with the core of GeoNS. These applications are in three different folders as independent projects as path_selection, range_control, and clustering. If you do not wish to compile any of the applications, simply comment the respective line in the main CMakeLists file. 

**Distributed Path Selection in millimeter-wave IAB networks**

**Transmission range control in topology management of wireless sensor networks (WNSs)**

**Distributed clustering via message-passing (fast local clustering (floc))**

## Future Developments.

Overall the code is stable. I keep developing on the develop branch and the master branch is always stable. I am open to suggestions/contributions for other established algorithms to add or other developments to support more use cases. My next step is to add neural networks and backpropagation to the RL agents.


## Visualization

The visualization of the network is done via simple SVG based outputs.


### Acknowledgements
Thanks to Mola Pahnadayan who inspired me always as a true programmer.

Code dude! :+1: :shipit:
