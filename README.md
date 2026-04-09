# Parsian SSL

[![Build Status](https://gitlab.com/ParsianSoftware/parsian_ssl/badges/master/pipeline.svg)](https://gitlab.com/ParsianSoftware/parsian_ssl/pipelines)
[![License: LGPL v3](https://img.shields.io/badge/License-LGPLv3-blue.svg)](LICENSE)

**Parsian SSL** is the open-source ROS-based software stack developed by the [Parsian](http://parsian-robot.ir) team from Amirkabir University of Technology (AUT) for competing in the [RoboCup Small Size League (SSL)](https://ssl.robocup.org/). It covers the full autonomy pipeline — from vision processing and world-model estimation to AI decision-making, motion planning, and low-level robot communication.

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Packages](#packages)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Building](#building)
- [Usage](#usage)
- [Contributing](#contributing)
- [Changelog](#changelog)
- [License](#license)
- [Maintainers](#maintainers)

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                       Parsian SSL Stack                      │
│                                                             │
│  ┌──────────────────┐      ┌──────────────────────────────┐ │
│  │  SSL Vision /    │      │        parsian_msgs           │ │
│  │  grSim / Referee │      │  (shared ROS message types)  │ │
│  └────────┬─────────┘      └──────────────────────────────┘ │
│           │                                                 │
│  ┌────────▼─────────────────────────┐                       │
│  │   parsian_protobuf_wrapper       │  Decode network       │
│  │  (Vision / Referee / grSim I/O)  │  Protobuf packets     │
│  └────────┬─────────────────────────┘                       │
│           │                                                 │
│  ┌────────▼─────────────────────────┐                       │
│  │      parsian_world_model         │  Kalman filter,       │
│  │  (ball & robot state estimation) │  sensor fusion        │
│  └────────┬─────────────────────────┘                       │
│           │                                                 │
│  ┌────────▼─────────────────────────┐                       │
│  │          parsian_ai              │  Game strategy,       │
│  │    (plays, behaviours, skills)   │  role assignment      │
│  └────────┬─────────────────────────┘                       │
│           │  (per-robot Task messages)                      │
│  ┌────────▼─────────────────────────┐                       │
│  │         parsian_agent            │  Path planning,       │
│  │  (motion planning & control)     │  motion control       │
│  └────────┬─────────────────────────┘                       │
│           │  (robot Command messages)                       │
│  ┌────────▼─────────────────────────┐                       │
│  │     parsian_communication        │  Serial encoding,     │
│  │  (robot hardware interface)      │  RF/USB dispatch      │
│  └──────────────────────────────────┘                       │
│                                                             │
│  ┌──────────────────┐  ┌──────────┐  ┌──────────────────┐  │
│  │  parsian_util    │  │parsian_  │  │ rqt_parsian_gui  │  │
│  │  (geometry/math) │  │  tools   │  │   (RQt monitor)  │  │
│  └──────────────────┘  └──────────┘  └──────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## Packages

| Package | Description |
|---------|-------------|
| **parsian_msgs** | All custom ROS messages, services, and actions used across the stack. |
| **parsian_util** | Shared utility library: geometry helpers, core data structures, and action primitives. |
| **parsian_protobuf_wrapper** | Decodes SSL-Vision, SSL-Refbox (referee), and grSim Protobuf UDP packets and republishes them as ROS topics. |
| **parsian_world_model** | Fuses multi-camera vision data using a Kalman filter to produce a consistent world model (robot poses, ball position, velocities). |
| **parsian_ai** | Multi-agent game AI: plays, behavioral trees, skills, and a strategy server. Outputs per-robot task assignments. |
| **parsian_agent** | Per-robot nodelet that reads task messages and runs path planning and motion control, emitting low-level command messages. |
| **parsian_communication** | Encodes robot commands into a binary protocol and dispatches them to the robots over a serial/RF link. |
| **parsian_tools** | Miscellaneous tooling and helper nodelets used across the stack. |
| **rqt_parsian_gui** | RQt-based graphical interface for monitoring the world model, strategy selection, and robot status. |

---

## Prerequisites

| Requirement | Version / Notes |
|-------------|-----------------|
| Ubuntu | 16.04 LTS (Xenial) recommended |
| [ROS](http://www.ros.org/install/) | Kinetic Kame |
| catkin build tools | `catkin_tools` |
| Qt 4 | `libqt4-dev` |
| Protobuf | `protobuf-compiler`, `libprotobuf-dev` |
| Eigen 3 | `libeigen3-dev` |
| libqjson | `libqjson-dev` |
| [qextserialport](https://github.com/qextserialport/qextserialport) | Built from source |
| Python 3 + pip | `matplotlib`, `scikit-learn`, `scipy` |

Install the system-level dependencies at once:

```bash
sudo apt-get update
sudo apt-get install -y \
    libqt4-dev libqjson-dev \
    protobuf-compiler libprotobuf-dev \
    libeigen3-dev \
    python3-pip python3-tk curl
sudo pip3 install --upgrade pip matplotlib sklearn scipy
```

---

## Installation

A convenience script handles workspace creation, repository cloning, and dependency setup:

```bash
sh -c "$(curl -fsSL https://gist.githubusercontent.com/mahi97/60295c82e21215701d42d4c1e679ac1f/raw/66662032274aa55888099138884748cd2a47f092/install.sh clone)"
```

For a **manual** setup:

```bash
# 1. Create and initialise a catkin workspace
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
catkin_init_workspace

# 2. Clone this repository (with submodules)
git clone --recurse-submodules https://github.com/mahi97/parsian_ssl.git

# 3. Install qextserialport
cd parsian_ssl/parsian_communication
git clone https://github.com/qextserialport/qextserialport.git temp
cd temp && qmake-qt4 && make && sudo make install
cd .. && rm -rf temp

# 4. Install Python dependencies
bash parsian_agent/deps.sh   # installs matplotlib, sklearn, scipy
```

---

## Building

```bash
cd ~/catkin_ws
catkin build --summarize
source devel/setup.bash
```

> **Tip:** Add `source ~/catkin_ws/devel/setup.bash` to your `~/.bashrc` to avoid sourcing it manually each session.

---

## Usage

### Launch the full stack (simulation with grSim)

```bash
# Terminal 1 – World model
roslaunch parsian_world_model worldmodel.launch

# Terminal 2 – AI
roslaunch parsian_ai ai.launch

# Terminal 3 – Agents (one nodelet per robot)
roslaunch parsian_agent agent.launch
```

### Coding style

The project uses [astyle](http://astyle.sourceforge.net/) for C++ formatting. Run the following command from the workspace root to format all C/C++ source files:

```bash
find -regextype egrep -regex '.*\.[ch](pp)?$' \
    -exec astyle '{}' -A2 --indent=spaces=4 --pad-oper --unpad-paren --pad-header --convert-tabs \;
```

---

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for the naming conventions, nodelet structure guidelines, and coding style rules before submitting a pull request.

---

## Changelog

See [CHANGELOG](CHANGELOG) for a summary of changes across releases.

---

## License

This project is licensed under the **GNU Lesser General Public License v3.0**. See [LICENSE](LICENSE) for the full text.

---

## Maintainers

- **Mohammad Mahdi Rahimi** — [mohammadmahdi76@gmail.com](mailto:mohammadmahdi76@gmail.com) / [mmrahimi@aut.ac.ir](mailto:mmrahimi@aut.ac.ir)

