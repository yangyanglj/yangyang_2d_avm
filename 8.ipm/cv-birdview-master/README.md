# opencv-Birdview
This repository features an image perspective transformation and text recognition tool.

Following is a summary of the main features:
- [x] Image cropping automation
- [x] Bird's eye view transformation
- [x] Optical character recognition

Run this on anything you want to digitize such as checks, receipts, business cards or media front covers:

![Example](https://raw.githubusercontent.com/cfanatic/cv-birdview/master/res/img.png)

## Requirements
Developed and tested on the following setup: 
- CMake 3.0.0
- OpenCV 3.4.2
- Boost 1.67.0
- Tesseract 4.0.0

## Installation
I recommend to use Visual Studio Code. If not, run the following commands:
```
cmake CMakeLists.txt
make
./Birdview
```

## Usage
Make sure to update the [**configuration file**](https://raw.githubusercontent.com/cfanatic/cv-birdview/master/res/config.ini.example) first.

The image above as an input should yield:

![OCR](https://raw.githubusercontent.com/cfanatic/cv-birdview/master/res/img_ocr.png)

![Transformation](https://raw.githubusercontent.com/cfanatic/cv-birdview/master/res/img_transform.png)
