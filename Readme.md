# dvmot
Deep Visual Multiple Object Tracking


## Prerequisites
* [darknet](https://github.com/AlexeyAB/darknet)
* [opencv3.4](https://github.com/opencv/opencv/tree/3.4)


## Usage
Compile and run dvmot
```
$ make
$ ./dvmot.out dvnmot.ini
```
## Pre-trained models

- Model: [yolov4](https://arxiv.org/abs/2004.10934), Dataset: MOT20
    - [Weights file](https://drive.google.com/file/d/1o9PNhYTKhWpY_ZAna2U5014BNKCxh5jm/view?usp=sharing)
    - [Class names](https://drive.google.com/file/d/1o9PNhYTKhWpY_ZAna2U5014BNKCxh5jm/view?usp=sharing)
    - [Config file](https://drive.google.com/file/d/1F43PWU8PtvLV7Gb7gtY5GBot-UVwwF_k/view?usp=sharing)

- Model: [yolov4](https://arxiv.org/abs/2004.10934), Dataset: MS COCO
    - [Weights file](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights)
    - [Class names](https://github.com/AlexeyAB/darknet/blob/master/data/coco.names)
    - [Config file](https://raw.githubusercontent.com/AlexeyAB/darknet/master/cfg/yolov4.cfg)
