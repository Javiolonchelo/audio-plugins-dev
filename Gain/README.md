# French Coconut Gain

## Introduction
This plugin is based on Adriana Proenza's cat: Coco. He's a very silly cat and I love him. He looks like he loves *croissants*, so I put him in Paris. I also love croissants.

## How to use

- **Left-click & Drag**: Changes the gain in decibels that is applied to the input signal.
- **Right-click & Drag**: Moves Coco to another position.
- **Mouse wheel**: Changes Coco's size.
- **Double left-click**: Resets the gain to 0 dB.
- **Double right-click**: Resets Coco's position and size.

## Under the hood

It's just a simple gain gain plugin which implements the equation:

$$y[n] = x[n] * k$$

Where $k$ is the linear gain value. As you can see, it's just a multiplication.

### Limitations of this approach

According to **Stefano D'Angelo** in [his talk at The Audio Programmer](https://youtu.be/KFEl_jYJK64?si=ihpKcpoSUGV-3Gdv), there are two main problems with this approach:

- **Fechner's Law.** The human ear is not linear, so a linear gain will not sound the same to us. We need to use a logarithmic scale for gain.
- **Zipper Noise.** The user interaction may cause abrupt gain changes, and this introduces noise. We need to use a smooth transition between gain values.

I highly recommend watching the video for a deeper understanding of these concepts. Also, Stefano's company Orastron maintains a super cool library called [Brickworks](https://github.com/orastron/brickworks).

### The chosen solution

To solve the first one, we use a logarithmic Gain parameter. This means we specify the gain in decibels (dB) instead of a linear value. The conversion is done using the following formula:

$$G = 10^{\frac{k}{20}}$$

To solve the second one, we use what is usually called a fixed time ramp. This means that in a fixed time window (in our case, 20 ms), we will get the new value of the gain by applying a linear interpolation between the current value and the new value.

TODO: Add the equation for fixed time ramp 

So, in the end, the actual processing looks like this:

TODO: Add the final equation 

> [!NOTE]
> For more information, check this video by The Audio Programmer: [JUCE Tutorial 18 - Smoothing Parameters With The Linear Smoothed Value Class](https://www.youtube.com/watch?v=2FkxKz37kHs).
