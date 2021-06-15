# dvmot
Deep Visual Multiple Object Tracking


Example: [social distance detector covid-19](https://youtu.be/T5GswupN_v4)


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
	- [Weights file](https://drive.google.com/file/d/1OdPvjVffOoqg077I38smfBjiRDUEeJjA/view?usp=sharing)
	- [Class names](https://drive.google.com/file/d/1DgLc2JesQgaeO0U8IsWwjXNuZ7U6fT_i/view?usp=sharing)
	- [Config file](https://drive.google.com/file/d/180_2lrUiVf2HlAe10f5fsf8quZt6HbB5/view?usp=sharing)

- Model: [yolov4](https://arxiv.org/abs/2004.10934), Dataset: MS COCO
	- [Weights file](https://github.com/AlexeyAB/darknet/releases/download/darknet_yolo_v3_optimal/yolov4.weights)
	- [Class names](https://github.com/AlexeyAB/darknet/blob/master/data/coco.names)
	- [Config file](https://raw.githubusercontent.com/AlexeyAB/darknet/master/cfg/yolov4.cfg)
