<img align="right" width="200" height="200" src="/images/logo.png">

# GeoNS

## Geometric Large Scale Wireless Network Simulator

GeoNS is an open-source platform to simulate large-scale wireless networks. Contrary to the existing network simulators, which are location-transparent in their indexing, GeoNS uses location-dependent indexing in storing nodes of the network. In other words, we propose to view a wireless network as a spatial database, which allows us to use highly sophisticated spatial data structures in storing nodes of the network.

* Run experiments in serial mode (helpful for debugging during development, or maybe sufficient for experiments).
* Run experiments fully parallelized.
* Synchronous or asynchronous reinforcement learning.
* Reinforcement learning is implemented with the help of *RLLib* (see https://github.com/HerveFrezza-Buet/RLlib).
* Modularity for easy modification / re-use of existing components.

### Implemented Algorithms
**Distributed Path Selection in millimeter-wave IAB networks**

**Transmission range control in topology management of wireless sensor networks (WNSs)**

### Getting Started
Follow the installation instructions below, and then get started in the examples folder.

## Future Developments.

Overall the code is stable. I am open to suggestions/contributions for other established algorithms to add or other developments to support more use cases. My next step is to add neural networks and backpropagation to the RL agents.


## Visualization

The visualization of the network is done via simple SVG based outputs.


## Installation

1.  Clone this repository to the local machine.

2. Install the anaconda environment appropriate for the machine.




### Acknowledgements
Thanks to Mola Pahnadayan who inspied me always as a true programmer.

Code dude!
