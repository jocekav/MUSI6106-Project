# Explanation of regression and current progress

The regression model has two outcome uses:
1. Choosing the tone of the amp, ie picking which preset amplifier to use.
2. Choosing how much (or any) of each effect.

The dataset I have found, “IDMT-SMT-Audio-Effects”, (https://www.idmt.fraunhofer.de/en/publications/datasets/audio_effects.html) has 20,592 monophonic guitar notes incorporating 11 different audio effects. We can also manually decide the appropriate settings on our amp for each of these samples (or a subset of these samples) to create our model that can classify types. 

Training the model will be done in Python.
I found an example that uses sklearn:
https://www.kdnuggets.com/2020/02/audio-data-analysis-deep-learning-python-part-1.html
(currently basing my code on this.)

Looking into "differential dsp" next.
