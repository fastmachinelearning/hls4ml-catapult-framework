## 'projects' directory
All hls4ml Catapult project directories should be put here. Unless the scripts have been changed, the name of every folder should follow one of the patterns below:
* `DenseXXXy`, where `XXX` is a number with **exactly** 3 digits (if the number has less than 3 digits, leading zeroes must be added) and `y` is any string. This corresponds to a standard (i.e. non-stress) model.
* `SDenseXXXy`, where `XXX` is a number with **exactly** 3 digits (if the number has less than 3 digits, leading zeroes must be added) and `y` is any string. This corresponds to a stress model.
* `EntireTemplate`. This corresponds to a special project designed for checking the compile-time correctness of the Catapult HLS C++ code.
