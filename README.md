<img align="right" width="205" height="109" src="/images/logo.png">

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


## Extended Notes

For more discussion, please see the [paper on Arxiv]().  If you use this repository in your work or otherwise wish to cite it, please make reference to the paper.



### Code Organization

The class types perform the following roles:

* **Runner** - Connects the `sampler`, `agent`, and `algorithm`; manages the training loop and logging of diagnostics.
  * **Sampler** - Manages `agent` / `environment` interaction to collect training data, can initialize parallel workers.
    * **Collector** - Steps `environments` (and maybe operates `agent`) and records samples, attached to `sampler`.
      * **Environment** - The task to be learned.
        * **Observation Space/Action Space** - Interface specifications from `environment` to `agent`.
      * **TrajectoryInfo** - Diagnostics logged on a per-trajectory basis.
  * **Agent** - Chooses control action to the `environment` in `sampler`; trained by the `algorithm`.  Interface to `model`.
    * **Model** - Torch neural network module, attached to the `agent`.
    * **Distribution** - Samples actions for stochastic `agents` and defines related formulas for use in loss function, attached to the `agent`.
  * **Algorithm** - Uses gathered samples to train the `agent` (e.g. defines a loss function and performs gradient descent).
    * **Optimizer** - Training update rule (e.g. Adam), attached to the `algorithm`.
    * **OptimizationInfo** - Diagnostics logged on a per-training batch basis.


### Acknowledgements
Thanks to Mola Pahnadayan who inspied me always as a true programmer.

Code dude!
