## Net019
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
3. If you want to work with the project directly in Catapult, run `compile` on this project and then open `Net019` in Catapult.
4. If you want to run the entire Catapult flow instead, run `synthesize` on this project.

If you choose not to use `synthesize`, remember to set the `REGISTER_THRESHOLD` directive in Catapult to a high value (e.g. 10240) before running `go assembly`.

### Performance and resource usage metrics as of 14/09/2021 with the same RF applied to all layers
The following graphs show various performance and resource usage metrics for Net019 synthesised with Vivado HLS, Catapult HLS with no blocks (i.e. option 1) and Catapult HLS with blocks (i.e. option 2).

In all cases, the synthesis was done by Vivado HLS 2020.1 or Catapult 10.5c/896140 for the Xilinx `xcvu9p-flgb2104-2L-e` FPGA with the clock period of 5 ns and 0.625 ns uncertainty. The cosimulation was done with the cosim function in Vivado HLS or QuestaSim 2020.4 and the resource usage metrics were obtained from the Vivado 2020.1 logic synthesis process.

The same ReuseFactor was applied to the entire model / all layers in the model.

#### Resource usage (LUTs, FFs, DSPs)
![LUT](https://user-images.githubusercontent.com/24892582/133286572-7715a779-f442-4c20-8569-ecad559ab0cf.png)

![FF](https://user-images.githubusercontent.com/24892582/133291499-72cc87ab-86c3-45d2-9003-2cea436bc05d.png)

![DSP](https://user-images.githubusercontent.com/24892582/133287349-7ca36837-b302-4e5d-afba-5fc312b63658.png)

#### Predicted performance
![Predicted latency](https://user-images.githubusercontent.com/24892582/133291522-64f89538-ab9c-466f-bb1e-b3ba4d11ff5e.png)

![Predicted throughput](https://user-images.githubusercontent.com/24892582/133291542-7c454b20-9b03-435c-81a3-39c037961d84.png)

#### Performance resulting from cosimulation
![Latency from cosimulation](https://user-images.githubusercontent.com/24892582/133291571-ffde6354-cf50-4a73-9b49-167984f770cf.png)

![Throughput from cosimulation](https://user-images.githubusercontent.com/24892582/133291592-7c06c252-16e1-4c41-a4e8-e2e19ca75118.png)

### Performance and resource usage metrics as of 14/09/2021 with different RF applied to layers
The following graphs show various performance and resource usage metrics for Net019 synthesised with Vivado HLS and Catapult HLS with blocks (i.e. option 2).

In all cases, the synthesis was done by Vivado HLS 2020.1 or Catapult 10.5c/896140 for the Xilinx `xcvu9p-flgb2104-2L-e` FPGA with the clock period of 5 ns and 0.625 ns uncertainty. The cosimulation was done with the cosim function in Vivado HLS or QuestaSim 2020.4 and the resource usage metrics were obtained from the Vivado 2020.1 logic synthesis process.

All activation layers had ReuseFactor = 1. For other layers, the notation "X / Y" means "ReuseFactor = X for the first Dense layer and ReuseFactor = Y for the second Dense layer".

#### Resource usage (LUTs, FFs, DSPs)
![LUT](https://user-images.githubusercontent.com/24892582/133293889-55df8dd2-a850-4355-9c9d-0300511e7359.png)

![FF](https://user-images.githubusercontent.com/24892582/133294007-4f3aedeb-8948-463f-9d26-03b5bbe80334.png)

![DSP](https://user-images.githubusercontent.com/24892582/133294017-9d173d53-bafd-48db-b290-ecffdcc0f9f8.png)

#### Predicted performance
![Predicted latency](https://user-images.githubusercontent.com/24892582/133294054-ee1acb1c-b080-4394-917a-dd524ca42cd7.png)

![Predicted throughput](https://user-images.githubusercontent.com/24892582/133294059-c35a2e08-017b-487c-9af9-617fddaf7d0b.png)

#### Performance resulting from cosimulation
![Latency from cosimulation](https://user-images.githubusercontent.com/24892582/133294106-15d1941f-c404-4967-9cf6-3262f90e0408.png)

![Throughput from cosimulation](https://user-images.githubusercontent.com/24892582/133294128-6adfd7f6-aa88-4ba5-9762-1f9173bca353.png)
