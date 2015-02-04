How-to Use I2S_demo example

The demo provides playback and record function.
Playback: The demo would play the audio wav file from the SD card.
Record: The demo would record the sound to the SD card.

The demo suppport 16bit, 32bit quantization level.
Supporting sample rate: 8K, 11.025k, 12k, 16k, 22.05k, 24k, 32k, 44.1k, 48k, 
96KHZ.

Example platform: SAI + SGTL5000 + sdcard

Examples of the command in shell:

play ii2s0: a:/<filename>.wav
record ii2s0: a:/<filename>.wav <time>:<sample rate>:<quantization>:<channel>
(For example: record ii2s0: a:/test.wav 5:44100:16:2)

The demo now can only play the standard PCM file, and the header of the file should be standard. You can get some sample wav file in:
http://www.cs.bath.ac.uk/~rwd/cardattrit.html and http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/Samples.html

Notice: Not all the audio files can be played by the demo. Actually most of them can't, as they are not PCM format or have no standard header.
An example which can play is the "Soundfile 1:  a basic minimum-header standard mono WAVE file." in the first website.

Notice: The i2s_demo is for the platform which has I2S module. 
