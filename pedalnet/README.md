﻿# PedalNet First Trial
## Setup
### Step 1
make sure you installed these dependency:
>torch==1.7.0
pytorch_lightning==1.1.0
numpy==1.19.4
scipy==1.5.4
matplotlib==3.3.3

>pytorch CUDA support

CUDA installation instruction can be found here:
https://pytorch.org/get-started/locally/

Only sucessfully tested on Windows 10

### Step 2
#### To train the model
    python train.py yourInput.wav yourGroundTruth.wav

#### To test the model
    python test.py --model=your_trained_model.ckpt

>Creates files y_test.wav, y_pred.wav, and x_test.wav, for the ground truth output, predicted output, and input signal respectively.

#### Export to JSON
    python export.py --model=your_trained_model.ckpt

