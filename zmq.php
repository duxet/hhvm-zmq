<?hh

class ZMQ {
}

class ZMQContext {

    /**
     * Build a new ZMQContext.
     * @return void
     */
    <<__Native>>
    public function __construct(int $io_threads = 1): void;

    /**
     * Construct a new ZMQ object. The extending class must call this method. 
     * The type is one of the ZMQ::SOCKET_* constants. 
     * Persistent id allows reusing the socket over multiple requests. 
     * If persistent_id parameter is specific the type parameter is ignored and the 
     * socket is of type that it was initially created with. Persistent context must 
     * be enabled for persistent_id to work. Setting incorrect socket type might 
     * cause failure later in connect/bind/setSockOpt.
     *
     * @param integer $type              The type of the socket
     * @param string  $persistent_id     The persistent id. Can be used to create
     *                                   persistent connections
     * @param function   $on_new_socket     Called when a new socket is created     
     * @throws ZMQException
     * @return ZMQSocket
     */
    public function getSocket(int $type, string $persistent_id = null, Closure $on_new_socket = null) : ZMQSocket
    {
        // Fix for "Warning: __construct() expects parameter 4 to be object, NULL given"
        if ($on_new_socket)
        {
            $socket = new ZMQSocket($this, $type, $persistent_id, $on_new_socket);
        }
        else
        {
            $socket = new ZMQSocket($this, $type, $persistent_id);
        }
        
        return $socket;
    }
    
    /**
     * Whether the context is persistent
     *
     * @return boolean
     */
    public function isPersistent()
    {
        return true;
    }
    
}

class ZMQSocket {
/**
     * Construct a new ZMQ object. The extending class must call this method. 
     * The type is one of the ZMQ::SOCKET_* constants. 
     * Persistent id allows reusing the socket over multiple requests. 
     * If persistent_id parameter is specific the type parameter is ignored and the 
     * socket is of type that it was initially created with. Persistent context must 
     * be enabled for persistent_id to work. Setting incorrect socket type might 
     * cause failure later in connect/bind/setSockOpt.
     *
     * @param ZMQContext $context           ZMQContext to build this object
     * @param integer    $type              The type of the socket
     * @param string     $persistent_id     The persistent id. Can be used to create
     *                                      persistent connections
     * @param function   $on_new_socket     Called when a new socket is created
     * @throws ZMQException
     * @return void
     */
    <<__Native>>
    public function __construct(ZMQContext $context, int $type, string $persistent_id = null, Closure $on_new_socket = null): void;

    /**
     * Sends a message to the queue. 
     *
     * @param string  $message  The message to send
     * @param integer $flags    self::MODE_NOBLOCK or 0
     * @throws ZMQException if sending message fails
     *
     * @return mixed
     */
    <<__Native>>
    public function send(string $message, int $flags = 0): mixed;

    /**
     * Receives a message from the queue.
     * 
     * @param integer $flags self::MODE_NOBLOCK or 0
     * @throws ZMQException if receiving fails.
     *
     * @return mixed
     */
    <<__Native>>
    public function recv(int $flags = 0): mixed;

    /**
     * Connect the socket to a remote endpoint. For more information about the dsn 
     * see http://api.zeromq.org/zmq_connect.html. By default the method does not 
     * try to connect if it has been already connected to the address specified by $dsn.
     *
     * @param string  $dsn   The connect dsn
     * @param boolean $force Tries to connect to end-point even if the object is already connected to the $dsn
     *
     * @throws ZMQException If connection fails
     * @return ZMQSocket
     */
    <<__Native>>
    public function connect(string $dsn, bool $force = false): ZMQSocket;

    /**
     * Disconnect the socket from a previously connected remote endpoint.
     *
     * @param string  $dsn   The disconnect dsn
     *
     * @throws ZMQException If disconnecting fails
     * @return ZMQSocket
     */
    <<__Native>>
    public function disconnect(string $dsn): ZMQSocket;

    /**
     *
     * Binds the socket to an end-point. For more information about the dsn see 
     * http://api.zeromq.org/zmq_connect.html. By default the method does not 
     * try to bind if it has been already bound to the address specified by $dsn.
     *
     * @param string  $dsn   The bind dsn
     * @param boolean $force Tries to bind to end-point even if the object is already bound to the $dsn
     *
     * @throws ZMQException if binding fails
     * @return ZMQSocket
     */
    <<__Native>>
    public function bind(string $dsn, bool $force = false): ZMQSocket;

    /**
     * Sets a socket option. For more information about socket options see 
     * http://api.zeromq.org/zmq_setsockopt.html
     *
     * @param integer $key   The option key
     * @param mixed   $value The option value
     *
     * @throws ZMQException
     * @return ZMQ
     */
    <<__Native>>
    public function setSockOpt(int $key, mixed $value): ZMQSocket;

    /**
     * Gets a socket option. This method is available if ZMQ extension
     * has been compiled against ZMQ version 2.0.7 or higher
     *
     * @since 0MQ 2.0.7
     * @param integer $key The option key
     *
     * @throws ZMQException
     * @return mixed
     */
    public function getSockOpt(int $key) {} 

    /**
     * Get endpoints where the socket is connected to. The return array
     * contains two sub-arrays: 'connect' and 'bind'
     *
     * @throws ZMQException
     * @return array 
     */
    public function getEndpoints() {}

    /**
     * Return the socket type. Returns one of ZMQ::SOCKET_* constants
     *
     * @throws ZMQException
     * @return integer 
     */
    <<__Native>>
    public function getSocketType(): int;
    
    /**
     * Whether the socket is persistent
     *
     * @return boolean
     */
    public function isPersistent() {
        return false;
    }
}

class ZMQException extends Exception {
}

class ZMQContextException extends ZMQException {
}

class ZMQSocketException extends ZMQException {
}
