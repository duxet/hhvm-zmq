<?hh

class ZMQ {

    /**
     * Publish-subscribe 
     * Compatible sockets: SOCKET_SUB
     */
    const SOCKET_PAIR = 0;

    /**
     * Publish-subscribe 
     * Compatible sockets: SOCKET_SUB
     */
    const SOCKET_PUB = 1;

    /**
     * Publish-subscribe 
     * Compatible sockets: SOCKET_PUB
     */    
    const SOCKET_SUB = 2;

    /**
     * Request-reply
     * Compatible sockets: SOCKET_REP
     */
    const SOCKET_REQ = 3;

    /**
     * Request-reply
     * Compatible sockets: SOCKET_REQ
     */
    const SOCKET_REP = 4;

    /**
     * TODO
     */
    const SOCKET_XREQ = 5;

    /**
     * TODO
     */
    const SOCKET_XREP = 6;

    /**
     * Parallelized pipeline
     * Compatible sockets: SOCKET_DOWNSTREAM
     */
    const SOCKET_UPSTREAM = 7;

    /**
     * Parallelized pipeline
     * Compatible sockets: SOCKET_UPSTREAM
     */
    const SOCKET_DOWNSTREAM = 8;
    
    /**
     * TODO
     */
    const SOCKET_PUSH = 8;

    /**
     * Set high water mark
     * Valuetype: integer
     */
    const SOCKOPT_HWM = 1;

    /**
     * Set low water mark
     * Valuetype: integer
     */
    const SOCKOPT_LWM = 2;

    /**
     * Set disk offload size
     * Valuetype: integer
     */
    const SOCKOPT_SWAP = 3;

    /**
     * Set I/O thread affinity 
     * Valuetype: integer
     */
    const SOCKOPT_AFFINITY = 4;

    /**
     * Set socket identity
     * Valuetype: string
     */
    const SOCKOPT_IDENTITY = 5;

    /**
     * Establish message filter
     * Valuetype: string
     */
    const SOCKOPT_SUBSCRIBE = 6;

    /**
     * Remove message filter
     * Valuetype: string
     */
    const SOCKOPT_UNSUBSCRIBE = 7;

    /**
     * Set multicast data rate
     * Valuetype: integer >= 0
     */
    const SOCKOPT_RATE = 8;

    /**
     * Set multicast recovery interval
     * Valuetype: integer >= 0
     */
    const SOCKOPT_RECOVERY_IVL = 9;

    /**
     * Control multicast loopback
     * Valuetype: integer >= 0
     */
    const SOCKOPT_MCAST_LOOP = 10;

    /**
     * Set kernel transmit buffer size
     * Valuetype: integer >= 0
     */
    const SOCKOPT_SNDBUF = 11;

    /**
     * Set kernel receive buffer size
     * Valuetype: integer >= 0
     */
    const SOCKOPT_RCVBUF = 12;
    
    /**
     * Receive multipart message
     */
    const SOCKOPT_RCVMORE = 13;
    
    const SOCKOPT_LINGER = 17;
    
    /**
     * Set on non-blocking mode
     */
    const MODE_NOBLOCK = 1;
     
    /**
     * Send multipart message
     */
    const MODE_SNDMORE = 2;
 
    /**
     * Track if the socket is readable
     */
    const POLL_IN = 1;

    /**
     * Track if the socket is writable
     */
    const POLL_OUT = 2;
    
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
     * @return ZMQ
     */
    <<__Native>>
    public function send(string $message, int $flags = 0): ZMQSocket;

    /**
     * Receives a message from the queue.
     * 
     * @param integer $flags self::MODE_NOBLOCK or 0
     * @throws ZMQException if receiving fails.
     *
     * @return string
     */
    public function recv($flags = 0) {}

    /**
     * Connect the socket to a remote endpoint. For more information about the dsn 
     * see http://api.zeromq.org/zmq_connect.html. By default the method does not 
     * try to connect if it has been already connected to the address specified by $dsn.
     *
     * @param string  $dsn   The connect dsn
     * @param boolean $force Tries to connect to end-point even if the object is already connected to the $dsn
     *
     * @throws ZMQException If connection fails
     * @return ZMQ
     */
    <<__Native>>
    public function connect(string $dsn, bool $force = false): ZMQSocket;

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
     * @return ZMQ
     */
    public function bind($dsn, $force = false) {}

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
    public function getSockOpt($key) {}    

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
    public function getSocketType() {}
    
    /**
     * Whether the socket is persistent
     *
     * @return boolean
     */
    public function isPersistent() {}
}

class ZMQException extends Exception {
}

class ZMQContextException extends ZMQException {
}

class ZMQSocketException extends ZMQException {
}
