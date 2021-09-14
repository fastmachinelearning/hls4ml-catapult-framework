## Dense019
This is a project implementing the standard Keras model number 019 from https://gist.github.com/maksgraczyk/2ded252bf471c94d43bfd836b3953a23, i.e. the one described by the Keras code below:
```
model = Sequential()
model.add(Dense(20, input_shape=(16,), name='fc1', activation='relu',
                kernel_initializer='random_normal', bias_initializer='random_normal'))
model.add(Dense(5, name='fc2', activation='softmax',
                kernel_initializer='random_normal', bias_initializer='random_normal'))
```

### Synthesis options
There are two ways of synthesising this model:
* **Option 1**: Fully unrolling all loops and pipelining `myproject()` (i.e. the main function) with II (initiation interval) = target ReuseFactor (RF).
* **Option 2**: Modelling every layer as an independent block communicating with the outside world using `ac_channel`, fully unrolling all loops and pipelining each block with II = target RF for that block.

Option 1 allows for setting RF for the entire model **only**, while option 2 allows for setting RF for each layer separately. Both ways come with their pros and cons:
* With option 1, Catapult can apply optimisations on an entire model, including in-between layers, at the cost of smaller RF flexibility and harder analysis for humans (due to no distinction between layers in statistics produced by Catapult).
* With option 2, there is larger RF flexibility, different optimisations can be set for different layers and analysis is easier for humans (due to a clear distinction between layers). The cost is a performance and resource overhead of using blocks along with potential difficulties to apply optimisations in-between layers.

In order to use option 1, you need to uncomment the (\*) region in `tcl/04_architect.tcl` and `myproject.cpp` along with commenting the (\*\*) region there. Similarly, to use option 2, you need to uncomment the (\*\*) region and comment the (\*) one.

Those two files have comments pointing exactly where the regions are and explaining how to set II/RF. You can also tweak the code itself and/or other directives if you wish.

#### Tweaks applied to mitigate the performance and resource overhead in option 2
By inspecting `tcl/04_architect.tcl` and `myproject.cpp`, you will see that the following optimisations have been applied to option 2 to mitigate as much as possible the performance and resource overhead of using blocks:
* `ac_channel` is flattened in all blocks (e.g. one 320-bit channel is used instead of ten 32-bit channels).
* `const_size_*` variable assignments are moved out of `myproject()` to the last block in the pipeline. Without this tweak, Catapult would not figure out automatically what optimisations should be applied to `myproject()`, leaving this task to be done manually.
* The `FIFO_DEPTH` directive is set to 0 in all blocks to reduce the FIFO structures to a minimum.
* The `PIPELINE_STALL_MODE` directive is set to `bubble` in the last layer so that it does not stall in case of no inputs. Without this tweak, the cosimulated latency is much larger than the predicted one.

This list is not guaranteed to contain the optimal optimisations, so you are welcome to research further possibilities of reducing the block overhead.

### Quick start
1. Choose which synthesis option you want to use (see the section above).
2. Comment and uncomment the (\*) and (\*\*) regions accordingly and set II/RF to values of your choice (or leave the default ones which are 1 everywhere).
3. If you want to work with the project directly in Catapult, run `compile` on this project and then open `Dense019` in Catapult.
4. If you want to run the entire Catapult flow instead, run `synthesize` on this project.

If you choose not to use `synthesize`, remember to set the `REGISTER_THRESHOLD` directive in Catapult to a high value (e.g. 10240) before running `go assembly`.

### Performance and resource usage metrics as of 14/09/2021 with the same RF applied to all layers
The following graphs show various performance and resource usage metrics for Dense019 synthesised with Vivado HLS, Catapult HLS with no blocks (i.e. option 1) and Catapult HLS with blocks (i.e. option 2).

In all cases, the synthesis was done for the Xilinx `xcvu9p-flgb2104-2L-e` FPGA with the clock period of 5 ns and 0.625 ns uncertainty. The same ReuseFactor was applied to the entire model / all layers in the model.

#### Resource usage (LUTs, FFs, DSPs)
![LUT](https://user-images.githubusercontent.com/24892582/133286572-7715a779-f442-4c20-8569-ecad559ab0cf.png)

![FF](https://user-images.githubusercontent.com/24892582/133289920-87ae0ad5-ea51-4d0a-9054-49be6400907b.png)

![DSP](https://user-images.githubusercontent.com/24892582/133287349-7ca36837-b302-4e5d-afba-5fc312b63658.png)

#### Predicted performance
![Predicted latency](https://user-images.githubusercontent.com/24892582/133290419-223405a3-7a31-4179-9f2c-3709f30daa52.png)

![Predicted throughput](https://user-images.githubusercontent.com/24892582/133290443-676ce406-9e17-451b-a157-5917308cbb21.png)

#### Performance resulting from cosimulation
![Latency from cosimulation](https://user-images.githubusercontent.com/24892582/133290489-7271b343-457e-45db-b0da-8b0fc7300bb0.png)

![Throughput from cosimulation](https://user-images.githubusercontent.com/24892582/133290504-3e5c1e53-2bec-495a-a191-98ec384a60fa.png)
