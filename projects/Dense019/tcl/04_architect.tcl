# Put here the code to be executed immediately before running 'go architect'
# in Catapult.

directive set /myproject/layer1/core -DESIGN_GOAL Area
directive set /myproject/layer4/core -DESIGN_GOAL Area
directive set /myproject/layer3/core -DESIGN_GOAL Area
directive set /myproject/layer2/core -DESIGN_GOAL Area

directive set /myproject/layer1/core/Product1 -UNROLL yes
directive set /myproject/layer1/core/Product2 -UNROLL yes
directive set /myproject/layer1/core/Accum1 -UNROLL yes
directive set /myproject/layer1/core/Accum2 -UNROLL yes
directive set /myproject/layer1/core/Result -UNROLL yes

directive set /myproject/layer2/core/Relu -UNROLL yes

directive set /myproject/layer3/core/Product1 -UNROLL yes
directive set /myproject/layer3/core/Product2 -UNROLL yes
directive set /myproject/layer3/core/Accum1 -UNROLL yes
directive set /myproject/layer3/core/Accum2 -UNROLL yes
directive set /myproject/layer3/core/Result -UNROLL yes

directive set /myproject/layer4/core/SoftmaxLatencyExp -UNROLL yes
directive set /myproject/layer4/core/SoftmaxLatencyCalc -UNROLL yes

directive set /myproject/layer1/core/main -PIPELINE_INIT_INTERVAL 1
directive set /myproject/layer2/core/main -PIPELINE_INIT_INTERVAL 1
directive set /myproject/layer3/core/main -PIPELINE_INIT_INTERVAL 1
directive set /myproject/layer4/core/main -PIPELINE_INIT_INTERVAL 1

directive set /myproject/layer2_out:cns -FIFO_DEPTH 0
directive set /myproject/layer3_out:cns -FIFO_DEPTH 0
directive set /myproject/layer4_out:cns -FIFO_DEPTH 0

directive set /myproject/layer4/core/main -PIPELINE_STALL_MODE bubble

# directive set /myproject/core/main -PIPELINE_INIT_INTERVAL 1
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
