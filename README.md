# Scope
This repo is our pointcloud visualization framework on RGB images. The poinvloud is color coded for identifying saliency regions and the code for the classifications is in this repo https://github.com/Stagakis/saliency-from-pointcloud. This work is part of our research on driver awareness using AR displays. Both the pointcloud and the image are generated from CARLA. Right now it is operated offline, but plans for online implementation are on the roadmap. 

# Resources

Download the resources (lidar pointcloud, images, etc) and extract them. The program by default checks in the top level directory of the project for resources0/ and resources1/ folder.

[Resources for 2 agents](https://drive.google.com/file/d/19EdAVKOxIghdybYxJUTDHZUsqUrL4EzB/view?usp=sharing)

# Dependencies

The project is striving to be portable, everything needed should be in deps folder.

# Installation and running

Until some issued with the commit history are fixed, I recommend you to use git shallow clone. After the cloning is complete, navigate to the project folder and run:
```
mkdir build
cd build
cmake ..
make
./ARCarla
```
