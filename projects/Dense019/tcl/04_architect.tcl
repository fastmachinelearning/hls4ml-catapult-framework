# Put here the code to be executed immediately before running 'go architect'
# in Catapult.


# Uncomment the region below (**) and comment the region afterwards (*) if you
# want to synthesise Dense019 as a connection of blocks. You can set IIs/RFs
# through PIPELINE_INIT_INTERVAL (see the comment inside the (**) region).


### Region (**) starts
# # Instruct Catapult to synthesise all layers/blocks with the area in mind.
# directive set /myproject/layer1/core -DESIGN_GOAL Area
# directive set /myproject/layer2/core -DESIGN_GOAL Area
# directive set /myproject/layer3/core -DESIGN_GOAL Area
# directive set /myproject/layer4/core -DESIGN_GOAL Area

# # Fully unroll all loops in all layers/blocks.
# directive set /myproject/layer1/core/Product1 -UNROLL yes
# directive set /myproject/layer1/core/Product2 -UNROLL yes
# directive set /myproject/layer1/core/Accum1 -UNROLL yes
# directive set /myproject/layer1/core/Accum2 -UNROLL yes
# directive set /myproject/layer1/core/Result -UNROLL yes

# directive set /myproject/layer2/core/Relu -UNROLL yes

# directive set /myproject/layer3/core/Product1 -UNROLL yes
# directive set /myproject/layer3/core/Product2 -UNROLL yes
# directive set /myproject/layer3/core/Accum1 -UNROLL yes
# directive set /myproject/layer3/core/Accum2 -UNROLL yes
# directive set /myproject/layer3/core/Result -UNROLL yes

# directive set /myproject/layer4/core/SoftmaxLatencyExp -UNROLL yes
# directive set /myproject/layer4/core/SoftmaxLatencyCalc -UNROLL yes

# # Set II/RF for specific layers/blocks here. PIPELINE_INIT_INTERVAL = X means
# # II = RF = X. Each layer/block may have a different value of II/RF. The II/RF
# # of the entire design is the maximum PIPELINE_INIT_INTERVAL amongst the blocks.
# directive set /myproject/layer1/core/main -PIPELINE_INIT_INTERVAL 1
# directive set /myproject/layer2/core/main -PIPELINE_INIT_INTERVAL 1
# directive set /myproject/layer3/core/main -PIPELINE_INIT_INTERVAL 1
# directive set /myproject/layer4/core/main -PIPELINE_INIT_INTERVAL 1

# # Reduce the FIFO structures in layers/blocks to a minimum.
# directive set /myproject/layer2_out:cns -FIFO_DEPTH 0
# directive set /myproject/layer3_out:cns -FIFO_DEPTH 0
# directive set /myproject/layer4_out:cns -FIFO_DEPTH 0

# # Make the last layer not stall in case of no inputs. The 'bubble' mode
# # includes extra optimisations/compression, see the Catapult documentation
# # for the details.
# #
# # Without setting this directive, the cosimulated latency is much larger than
# # the predicted one.
# directive set /myproject/layer4/core/main -PIPELINE_STALL_MODE bubble
### Region (**) ends


# Uncomment the region below (*) and comment the region beforehand (**) if you
# want to synthesise Dense019 as an entire model (i.e. without using any blocks).
# You can set II/RF through PIPELINE_INIT_INTERVAL (see the comment inside the
# (*) region).


### Region (*) starts
# # Set II/RF for the entire model here. PIPELINE_INIT_INTERVAL = X means
# # II = RF = X.
# directive set /myproject/core/main -PIPELINE_INIT_INTERVAL 1

# # Fully unroll all loops.
# directive set /myproject/core/Product1 -UNROLL yes
# directive set /myproject/core/Product2 -UNROLL yes
# directive set /myproject/core/Accum1 -UNROLL yes
# directive set /myproject/core/Accum2 -UNROLL yes
# directive set /myproject/core/Result -UNROLL yes
# directive set /myproject/core/Relu -UNROLL yes
# directive set /myproject/core/Product1#1 -UNROLL yes
# directive set /myproject/core/Product2#1 -UNROLL yes
# directive set /myproject/core/Accum1#1 -UNROLL yes
# directive set /myproject/core/Accum2#1 -UNROLL yes
# directive set /myproject/core/Result#1 -UNROLL yes
# directive set /myproject/core/SoftmaxLatencyExp -UNROLL yes
# directive set /myproject/core/SoftmaxLatencyCalc -UNROLL yes
### Region (*) ends


# DO NOT LEAVE BOTH REGIONS UNCOMMENTED!
