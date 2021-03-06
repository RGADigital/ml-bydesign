# DRforBrand

## Goals
* Understanding how image recognition and classification works by machine learning.
* Apply image recognition & classification technology in your work.

## Understanding concept of machine learning for image recognition ( 30 mins )
* [MNIST](http://yann.lecun.com/exdb/mnist/)
 * [MNIST 8 training data](http://cs.nyu.edu/~roweis/data/mnist_train8.jpg)
 * [How MNIST works](https://ml4a.github.io/ml4a/neural_networks/)
* [ImageNet](http://image-net.org/)
 * The classification task is made up of 1.2 million images in the training set, each labeled with one of 1000 categories that cover a wide variety of objects, animals, scenes, and even some abstract geometric concepts such as “hook”, or “spiral”.


## Understanding dimensionality reduction. ( 30 mins )
* [t-sne](https://lvdmaaten.github.io/tsne/)
 * [t-sne shape classification example](https://lvdmaaten.github.io/tsne/examples/20news_tsne.jpg)
 * [t-sne MNIST example](https://lvdmaaten.github.io/tsne/examples/mnist_tsne.jpg)

## Dependencies
* XCode
* [openframeworks](https://www.openframeworks.cc)
* [ofxAssignment](https://github.com/kylemcdonald/ofxAssignment)
* [ofxCcv](https://github.com/kylemcdonald/ofxCcv)
* [ofxTSNE](https://github.com/genekogan/ofxTSNE)

## Setup (OS X)
1. Install Open Frameworks from [here](https://www.openframeworks.cc)
2. clone ml-bydesign folder at the same level of addons folder inside the openframeworks directory
3. Install the required addons by cloning them into the addons folder
    - [ofxAssignment](https://github.com/kylemcdonald/ofxAssignment)
    - [ofxCcv](https://github.com/kylemcdonald/ofxCcv)
    - [ofxTSNE](https://github.com/genekogan/ofxTSNE)
4. from the open frameworks root directory, `cd ml-bydesign/jin/DRforBrand`
5. sh setup.sh
6. `cd bin/data/instagram-scraper`
7. `pip install instagram-scraper`
8. Open ml-bydesign/jin/DRforBrand/Project.xcconfig with Xcode. Open Framworks XCode reference [here](http://openframeworks.cc/setup/xcode/)

## Usage
1. `cd ml-bydesign/jin/DRforBrand/bin/data/instagram-scraper`
2. `instagram-scraper <brand1>,<brand2>` (ex: `instagram-scraper realdonaldtrump,hillaryclinton`)
3. In Xcode top menu bar, go to product > scheme > edit scheme
4. Enter `--args --arguments <brand1> <brand2> <grid dimension>` (ex: `--args --arguments realdonaldtrump hillaryclinton 35`)
5. Hit "close"
6. In top menu, make sure the project to build is set for "DRForBrand Release" (it sometimes defaults to openFrameworks)
7. Build and run

Two image grids will be generated - one without any tinting, and one with each brand tinted. The images will be saved to the bin/data directory.
<brand1> will be tinted blue. <brand2> will be tinted red.


**Notes:**

- After the first build, you can run via the command line, from within the `ml-bydesign/jin/bin/` directory with:
`open -n ./DRforBrand.app/ --args --arguments <brand1> <brand2> <dimension>`
-  large grid dimensions will require a lot of processing and may crash the program. We've seen good results with dimensions between 35 and 60.

## Code walk through ( 30 mins )

## Download images from Instagram ( 20 mins )

## Example outputs
<img width="1118" alt="rga_profile_gray" src="https://cloud.githubusercontent.com/assets/333218/20449506/d1832862-adb7-11e6-934d-e6ede5a4d12f.png">
![01_tsne_grid_tiffany](https://cloud.githubusercontent.com/assets/333218/20449625/921dd752-adb8-11e6-8e71-a0d00c6abb05.png)
![02_tsne_grid_nike](https://cloud.githubusercontent.com/assets/333218/20449635/9cc4a366-adb8-11e6-8419-709c8d2ecbc3.png)
![04_tsne_grid_adidas](https://cloud.githubusercontent.com/assets/333218/20449643/aa21dc68-adb8-11e6-99da-6f76ecb55ebd.png)
![07_tsne_grid_nike_adidas_tint](https://cloud.githubusercontent.com/assets/333218/20449655/b52310dc-adb8-11e6-80fe-b2c250c39d8c.png)

## Bring your ideas
