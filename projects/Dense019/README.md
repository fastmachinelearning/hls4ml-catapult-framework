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
* With option 2, there is larger RF flexibility, different optimisations can be set for different layers and analysis is easier for humans (due to a clear distinction between layers). The cost is a performance and resource overhead of using blocks (tweaks have been applied to mitigate this as much as possible) along with potential difficulties to apply optimisations in-between layers.

In order to use option 1, you need to uncomment the (\*) region in `tcl/04_architect.tcl` and `myproject.cpp` along with commenting the (\*\*) region there. Similarly, to use option 2, you need to uncomment the (\*\*) region and comment the (\*) one.

Those two files have comments pointing exactly where the regions are and explaining how to set II/RF. You can also tweak the code itself and/or other directives if you wish.

### Quick start
1. Choose which synthesis option you want to use (see the section above).
2. Comment and uncomment the (\*) and (\*\*) regions accordingly and set II/RF to values of your choice (or leave the default ones which are 1 everywhere).
3. If you want to work with the project directly in Catapult, run `compile` on this project and then open `Dense019` in Catapult.
4. If you want to run the entire Catapult flow instead, run `synthesize` on this project.

If you choose not to use the framework tools, remember to set the `REGISTER_THRESHOLD` directive in Catapult to a high value (e.g. 10240) before running `go assembly`.
