## Dense019
This is a project implementing the standard Keras model number 019 from https://gist.github.com/maksgraczyk/2ded252bf471c94d43bfd836b3953a23, i.e. the one described by the Keras code below:
```
model = Sequential()
model.add(Dense(20, input_shape=(16,), name='fc1', activation='relu',
                kernel_initializer='random_normal', bias_initializer='random_normal'))
model.add(Dense(5, name='fc2', activation='softmax',
                kernel_initializer='random_normal', bias_initializer='random_normal'))
```
