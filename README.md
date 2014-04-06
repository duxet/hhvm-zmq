hhvm-zmq
========

ZMQ extension for HHVM. Shall be compatible with [existing PHP binding](http://php.net/zmq).


## Building and installation

~~~
export HPHP_HOME=/path/to/hhvm
./build.sh
~~~

Please remember that first you need to build HHVM from source. If you need help with that, take a look at [HHVM Wiki](https://github.com/facebook/hhvm/wiki).

Also: don't forgot about installing [libzmq](https://github.com/zeromq/libzmq). Currently only versions >= 4.0 are supported. 

## Tests

~~~
./test.sh
~~~
