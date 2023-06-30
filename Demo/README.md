# SARIF

## Overview

This repository contains a binary demo for the paper "Software Architecture Recovery with Information Fusion". The executable SARIF is compiled for **LINUX** platform. Source code will be released after receiving permission from our industrial collaborators. 

#### Environment Requirements
**JRE >= 8.0  and libjansson is required** (by external tools like Depends).

Quick Setup for Ubuntu 18.04:
    sudo apt update
    sudo apt install libjansson-dev
    sudo apt install openjdk-11-jre (optional)

#### Quick Test
You may enter the demo folder and run the following command to recover Distributed Camera's architecture with SARIF:

    ./sarif_demo distributed_camera/ --gt distributed_camera_gt.json

SARIF normally takes around 1 minute for the recovery, and you can find the recovery results in the logged result folder. The recovered architecture should be identical to our published data.


The *Quick Setup* and *Quick Test* were tested in a brand-new WSL Ubuntu-18.04. If this quick guild still does not work, please refer to the detailed guide and common issues in the next section.

## Details 

### Data

#### Data formats
- cluster_result.json: default clustering result format for SARIF. You may find a list of groups under the 'structure' key. Each group represents a cluster and consists of its corresponding files.

- *.rsf: default clustering result format for baseline outputs. Each line of the file declares the group of a file. 

Other data formats should be default to understand.



#### Target projects
We only included *Distributed Camera* in this Repo (Demo/) since it is the smallest one among the target projects. If you want to test SARIF with the other projects, please refer to the following links:

- Bash-4.2: https://ftp.gnu.org/gnu/bash/bash-4.2.tar.gz
- ArchStudio4: https://github.com/isr-uci-edu/ArchStudio4
- Distributed Camera: https://gitee.com/openharmony/distributed_camera (Commit 46ff87)
- Drivers Framework: https://gitee.com/openharmony/distributed_camera (Commit 0e196f)
- OODT-0.2: https://github.com/apache/oodt (Commit e927bc) 
- Hadoop-0.19.0: https://github.com/apache/hadoop (Commit f9ca84)
- ITK: https://github.com/InsightSoftwareConsortium/ITK (Commit 01661cc)

## Demo 

### Help Information

    usage: SARIF [-h] [-g  [...]] [-o] [--cache_dir] [-s  [...]] [-r] datapath [datapath ...]

    Clustering software projects.

    positional arguments:
    datapath              path to the input project folder

    options:
    -h, --help            show this help message and exit
    -g  [ ...], --gt  [ ...]
                            path to the ground truth json file
    -o , --out_dir        path to the result folder
    --cache_dir           cache path
    -s  [ ...], --stopword_file  [ ...]
                            paths to external stopword lists
    -r , --resolution     resolution parameter, affecting the final cluster size.

Normally, you only need to change *datapath*, *--gt* and *--resolution*.
 

### Common Issues

- You must test SARIF with the *ext_tools* folder in your CWD since the path is hard-coded in SARIF.
- If SARIF logs contain printed error messages from *ctags* or *Depends* (except for warnings about dependency extraction), please check if you have set up their environment correctly. You can manually test these tools without any parameters to see if they can output their help messages successfully.

