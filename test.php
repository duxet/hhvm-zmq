<?php

$host = 'tcp://localhost:5555';

function send_message($host)
{

    $context = new ZMQContext();

    $socket = $context->getSocket(ZMQ::SOCKET_PUSH, 'test');
    $socket->setSockOpt(ZMQ::SOCKOPT_LINGER, 100);

    $socket->connect($host);

    $socket->send(json_encode(array(
        'message' => 'sent from hhvm-zmq',
    )));

    print("message sent\n");
}

print("zmq extension loaded: ". extension_loaded("zmq") ."\n");

for ($i = 0; $i < 10; $i++) {
    send_message($host);
}
