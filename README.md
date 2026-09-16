# DFRCore (Dialable Fidelity and Resolution) Core simulation framework

DFRCore is designed to be a discrete simulation event (DES) environment that supports modeling objects with different levels of fidelity and supporting different levels of resolution.  While not modeling the actual entities, its the base environment to be extended to support that vision and supporting other efforts.

Its envisioned use-case is supporting a large entity (lifeforms, vehicles) count simulation while enabling dynamic modeling fidelity and greater optimization thur the use of multi-resolution modeling.  It should also support execution in either batch-mode (as fast as possible, true DES mode) or in a real-time federation with inputs injected from external sources (such as a DIS or HLA real-time federation).

The process of extending the framework is planned to be a refactoring of another effort and distilling the useful parts into this package.  

- Package Includes
    1. Application object and supporting logger
    2. Event definition and event que
    3. Simulation Clock (including both As-Fast-As-Possible or Wall-Clock constrained)

## TODOs All the items in our design

Create a list of future or currently broken items that needs to be addressed.

- Doxygen - documentation not auto building in the cmake system
    1. It appears that the HTML header and Footer sets in the doxygen.cmake file need to be commented out.  Still need to understand this issue.
    2. Its unclear if this can be built inside of VSCode OR must be done at command line.  Using the command line "cmake --build build --target doxygen" as top level.

- Determine utility of the event class
    1. The prority que for events supports fast insert/extraction
    2. Does the consumer need access to event to adjust nextTime?  (believe yes)

- Expand Package Support
    1. Design the SimObject Class.  
    2. Design and support SimObjects adjusting their events and times
    3. Design the synthetic environment and terrain implementation
    4. Design the coordinate system or systems

## How to Install / Build / Etc

TBD....

### Package Installation

    1. Download the package
    2. Install the package: sudo apt install ./<packageName>

### Source Package Usage

This section describes how to get the software package for improvements or modifications.
TODO...

## Back Story

This project supports two personal projects of mine (as a newly retired ModSim supporter).  The first is to provide a sharable set of base capabilities that will support my personal simulation project.  The project that I'm designing and scoping is an entity based force-on-force simulation that supports multiple levels of both fidelity and resolution.  Useful features will be refactored and migrated into this package.

The second effort of this project is to enable learning new development tools, processes, and deployment concepts. While the concepts of developing software are well known, the tools and processes that I used for the last 30 years are outdated and very manual.  This package is my attempt to "sharpen my saw".
