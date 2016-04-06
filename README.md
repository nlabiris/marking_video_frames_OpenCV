# Marking video frames using OpenCV

### Abstract

In this work, we constructed various algorithms in order to extract frames from videos using [OpenCV Computer Vision library](http://opencv.org/). The results are saved as a text file as well as a video file that consists of the extracted frames only.

Here you can download the [text](https://github.com/nlabiris/marking_video_frames_OpenCV/blob/master/Labiris_Nikolaos_2744_Ptixiaki.pdf) for the thesis. (It is written in the Greek language only)

### Compile

You can use the `Makefile` in order to compile all the algorithms at once

```shell
Usage: make all                 # compile all algorithms
       make <algorithm_name>    # compile the source code
                                # for a specific algorithm
       make clean               # delete any object files
```
where `algorithm_name` choose one of the following:
- absdiff
- comprdiff
- ecrdiff_v1
- ecrdiff_v2
- histdiff

This Makefile calls a series of other Makefiles in order to compile all the source code

### Usage

For each algorithm there is a set of executables

1. `<algorithm_name>_export_images`: Exports video frames as images in PNG format (marked only). This images can be pass to ffmpeg in order to construct the resulting video.
2. `<algorithm_name>_pipe_pixels`: Sends BGR (24 bpp) frames to stdout. For example, you can pipe the output to ffmpeg in order to write video having more choices and control over the encoding procedure.
3. `<algorithm_name>_write_video`: Write marked frames in a video files using OpenCV. The resulting video includes only marked frames and has the duration of the original video.

1. `./<algorithm_name>_export_images <input_video_file> <output_TXT_file>`
2. `./<algorithm_name>_pipe_pixels <input_video_file> <output_TXT_file>`
3. `./<algorithm_name>_write_video <input_video_file> <output_video_file> <output_TXT_file>`

> The `<output_TXT_file>` contains the results of the execution of the algorithm, i.e. marked frames, video stats etc.


### License

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/4.0/">Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License</a>.
