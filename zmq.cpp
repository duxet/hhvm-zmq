#include "hphp/runtime/base/base-includes.h"
#include "hphp/util/string-vsnprintf.h"

#include "zmq.h"
#include "zmq_common.h"

namespace HPHP {
    
const StaticString
  s_code("code"),
  s_message("message"),
  s_ZMQ("ZMQ"),
  s_ZMQSocketException("ZMQSocketException"),
        
  s_SOCKET_PAIR("SOCKET_PAIR"),
  s_SOCKET_PUB("SOCKET_PUB"),
  s_SOCKET_SUB("SOCKET_SUB"),
  
  #if ZMQ_VERSION_MAJOR >= 3
    s_SOCKET_XSUB("SOCKET_XSUB"),
    s_SOCKET_XPUB("SOCKET_XPUB"),
  #endif

  s_SOCKET_REQ("SOCKET_REQ"),
  s_SOCKET_REP("SOCKET_REP"),
  s_SOCKET_XREQ("SOCKET_XREQ"),
  s_SOCKET_XREP("SOCKET_XREP"),
  s_SOCKET_PUSH("SOCKET_PUSH"),
  s_SOCKET_PULL("SOCKET_PULL"),
  s_SOCKET_DEALER("SOCKET_DEALER"),
  s_SOCKET_ROUTER("SOCKET_ROUTER"),
  
  #if ZMQ_MAJOR_VERSION >= 4
    s_SOCKET_STREAM("SOCKET_STREAM"),
  #endif

  s_SOCKET_UPSTREAM("SOCKET_UPSTREAM"),
  s_SOCKET_DOWNSTREAM("SOCKET_DOWNSTREAM"),

  #if ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 0
    s_MODE_SNDLABEL("MODE_SNDLABEL"),
  #endif

  s_POLL_IN("POLL_IN"),
  s_POLL_OUT("POLL_OUT"),

  s_MODE_SNDMORE("MODE_SNDMORE"),
  s_MODE_NOBLOCK("MODE_NOBLOCK"),
  s_MODE_DONTWAIT("MODE_DONTWAIT"),

  s_DEVICE_FORWARDER("DEVICE_FORWARDER"),
  s_DEVICE_QUEUE("DEVICE_QUEUE"),
  s_DEVICE_STREAMER("DEVICE_STREAMER"),

  s_ERR_INTERNAL("ERR_INTERNAL"),
  s_ERR_EAGAIN("ERR_EAGAIN"),
  s_ERR_ENOTSUP("ERR_ENOTSUP"),
  s_ERR_EFSM("ERR_EFSM"),
  s_ERR_ETERM("ERR_ETERM"),

  s_SOCKOPT_HWM("SOCKOPT_HWM"),
  s_SOCKOPT_SNDHWM("SOCKOPT_SNDHWM"),
  s_SOCKOPT_RCVHWM("SOCKOPT_RCVHWM"),
  s_SOCKOPT_AFFINITY("SOCKOPT_AFFINITY"),
  s_SOCKOPT_IDENTITY("SOCKOPT_IDENTITY"),
  s_SOCKOPT_RATE("SOCKOPT_RATE"),
  s_SOCKOPT_RECOVERY_IVL("SOCKOPT_RECOVERY_IVL"),
  s_SOCKOPT_SNDBUF("SOCKOPT_SNDBUF"),
  s_SOCKOPT_RCVBUF("SOCKOPT_RCVBUF"),
  s_SOCKOPT_LINGER("SOCKOPT_LINGER"),
  s_SOCKOPT_RECONNECT_IVL("SOCKOPT_RECONNECT_IVL"),
  s_SOCKOPT_RECONNECT_IVL_MAX("SOCKOPT_RECONNECT_IVL_MAX"),
  s_SOCKOPT_BACKLOG("SOCKOPT_BACKLOG"),
  s_SOCKOPT_MAXMSGSIZE("SOCKOPT_MAXMSGSIZE"),
  s_SOCKOPT_SUBSCRIBE("SOCKOPT_SUBSCRIBE"),
  s_SOCKOPT_UNSUBSCRIBE("SOCKOPT_UNSUBSCRIBE"),
  s_SOCKOPT_TYPE("SOCKOPT_TYPE"),
  s_SOCKOPT_RCVMORE("SOCKOPT_RCVMORE"),
  s_SOCKOPT_FD("SOCKOPT_FD"),
  s_SOCKOPT_EVENTS("SOCKOPT_EVENTS"),
  s_SOCKOPT_SNDTIMEO("SOCKOPT_SNDTIMEO"),
  s_SOCKOPT_RCVTIMEO("SOCKOPT_RCVTIMEO"),
  s_SOCKOPT_IPV4ONLY("SOCKOPT_IPV4ONLY"),
  s_SOCKOPT_LAST_ENDPOINT("SOCKOPT_LAST_ENDPOINT"),
  s_SOCKOPT_TCP_KEEPALIVE("SOCKOPT_TCP_KEEPALIVE"),
  s_SOCKOPT_TCP_KEEPALIVE_IDLE("SOCKOPT_TCP_KEEPALIVE_IDLE"),
  s_SOCKOPT_TCP_KEEPALIVE_CNT("SOCKOPT_TCP_KEEPALIVE_CNT"),
  s_SOCKOPT_TCP_KEEPALIVE_INTVL("SOCKOPT_TCP_KEEPALIVE_INTVL"),
  s_SOCKOPT_TCP_ACCEPT_FILTER("SOCKOPT_TCP_ACCEPT_FILTER"),
  s_SOCKOPT_DELAY_ATTACH_ON_CONNECT("SOCKOPT_DELAY_ATTACH_ON_CONNECT"),
  s_SOCKOPT_XPUB_VERBOSE("SOCKOPT_XPUB_VERBOSE"),
  s_SOCKOPT_ROUTER_RAW("SOCKOPT_ROUTER_RAW"),
  s_SOCKOPT_IPV6("SOCKOPT_IPV6"),
  s_SOCKOPT_PLAIN_SERVER("SOCKOPT_PLAIN_SERVER"),
  s_SOCKOPT_PLAIN_USERNAME("SOCKOPT_PLAIN_USERNAME"),
  s_SOCKOPT_PLAIN_PASSWORD("SOCKOPT_PLAIN_PASSWORD"),
  s_SOCKOPT_CURVE_SERVER("SOCKOPT_CURVE_SERVER"),
  s_SOCKOPT_CURVE_PUBLICKEY("SOCKOPT_CURVE_PUBLICKEY"),
  s_SOCKOPT_CURVE_SECRETKEY("SOCKOPT_CURVE_SECRETKEY"),
  s_SOCKOPT_CURVE_SERVERKEY("SOCKOPT_CURVE_SERVERKEY"),
  s_SOCKOPT_PROBE_ROUTER("SOCKOPT_PROBE_ROUTER"),
  s_SOCKOPT_REQ_CORRELATE("SOCKOPT_REQ_CORRELATE"),
  s_SOCKOPT_REQ_RELAXED("SOCKOPT_REQ_RELAXED"),
  s_SOCKOPT_CONFLATE("SOCKOPT_CONFLATE"),
  s_SOCKOPT_ZAP_DOMAIN("SOCKOPT_ZAP_DOMAIN");
    
template<class T>
ALWAYS_INLINE
static T* getResource(const Object& obj, const char* varName) {
  auto var = obj->o_get(varName, true, s_zmqsocket.get());
  
  if (var.getType() == KindOfNull) {
    return nullptr;
  }
  
  return var.asCResRef().getTyped<T>();
}

ALWAYS_INLINE
static Variant setVariable(const Object& obj, const char* varName, const Variant& varValue) {
  return obj->o_set(varName, varValue, s_zmqsocket.get());
}

static void throwException(const String& clsName, int code, const char *fmt, ...) {
    Class* cls = Unit::loadClass(clsName.get());
    ObjectData* obj = ObjectData::newInstance(cls);

    obj->o_set(s_code, code, clsName);
    
    std::string msg;
    
    va_list ap;
    va_start(ap, fmt);
    string_vsnprintf(msg, fmt, ap);
    va_end(ap);
    
    obj->o_set(s_message, msg, clsName);
 
    throw Object(obj);
}

// ZMQContext

static void HHVM_METHOD(ZMQContext, __construct, int64_t io_threads) {
  ContextData *context = ContextData::GetPersistent(io_threads);
  
  if (context == nullptr) {
    context = new ContextData(io_threads);
  }

  ContextData::SetPersistent(io_threads, context);
  this_->o_set(s_context, context, s_zmqcontext.get());
}

// ZMQSocket

static void HHVM_METHOD(ZMQSocket, __construct, const Object& zmqcontext, int64_t type, const String& persistent_id, const Object& on_new_socket) {
  auto context = get_context(zmqcontext);
  auto socket = NEWOBJ(SocketData)(context->get(), type);

  setVariable(this_, "socket", Resource(socket));
}

static Object HHVM_METHOD(ZMQSocket, bind, const String& dsn, bool force) {
  auto socket = getResource<SocketData>(this_, "socket");

  int result = zmq_bind(socket->get(), dsn.c_str());

  if (result != 0) {
      throwException(s_ZMQSocketException, errno, "Failed to bind the ZMQ: %s", zmq_strerror(errno));
  }
  
  return this_;
}

static Object HHVM_METHOD(ZMQSocket, connect, const String& dsn, bool force) {
  auto socket = getResource<SocketData>(this_, "socket");

  int result = zmq_connect(socket->get(), dsn.c_str());

  if (result != 0) {
    throwException(s_ZMQSocketException, errno, "Error creating socket: %s", zmq_strerror(errno));
  }
  
  return this_;
}

static Object HHVM_METHOD(ZMQSocket, disconnect, const String& dsn) {
  auto socket = getResource<SocketData>(this_, "socket");

  int result = zmq_disconnect(socket->get(), dsn.c_str());

  if (result != 0) {
    throwException(s_ZMQSocketException, errno, "Failed to disconnect the ZMQ socket: %s", zmq_strerror(errno));
  }
  
  return this_;
}

static Variant HHVM_METHOD(ZMQSocket, recv, int64_t mode) {
  auto socket = getResource<SocketData>(this_, "socket");

  zmq_msg_t msg;
  int result = zmq_msg_init(&msg);
  
  if (result != 0) {
    throwException(s_ZMQSocketException, errno, "Failed to initialize message structure: %s", zmq_strerror(errno));
  }
  
  result = zmq_msg_recv(&msg, socket->get(), mode);
  int errno_ = errno;
  
  if (result == -1) {
    zmq_msg_close(&msg);  
      
    if (errno_ == EAGAIN) {
      return false;
    }     
      
    throwException(s_ZMQSocketException, errno, "Failed to receive message: %s", zmq_strerror(errno));
  }
  
  std::string data((const char*) zmq_msg_data(&msg), zmq_msg_size(&msg));
  
  zmq_msg_close(&msg);
  
  return data;
}

static Variant HHVM_METHOD(ZMQSocket, send, const String& message, int64_t mode) {
  auto socket = getResource<SocketData>(this_, "socket");

  int result = zmq_send(socket->get(), message.c_str(), message.size() + 1, mode);

  if (result == -1) {
    if (errno == EAGAIN) {
      return false;
    }  
      
    throwException(s_ZMQSocketException, errno, "Failed to send message: %s", zmq_strerror(errno));
  }

  return this_;
}

static Object HHVM_METHOD(ZMQSocket, setSockOpt, int64_t key, const Variant& value) {
  auto socket = getResource<SocketData>(this_, "socket");

  String sValue = value.toString();
  
  int result = zmq_setsockopt(socket->get(), key, sValue.c_str(), sValue.size() + 1);

  if (result != 0) {
    throwException(s_ZMQSocketException, errno, "Failed to set socket option: %s", zmq_strerror(errno));
  }
  
  return this_;
}

static int64_t HHVM_METHOD(ZMQSocket, getSocketType) {
  auto socket = getResource<SocketData>(this_, "socket");

  int type;
  size_t type_size;
  type_size = sizeof(int);
  
  int result = zmq_getsockopt(socket->get(), ZMQ_TYPE, &type, &type_size);

  if (result != -1) {
      return type;
  }

  return -1;
}

static class ZMQExtension : public Extension {
 public:
  ZMQExtension() : Extension("zmq") {}
  virtual void moduleInit() {
    HHVM_ME(ZMQContext, __construct);
    
    HHVM_ME(ZMQSocket, __construct);
    HHVM_ME(ZMQSocket, bind);
    HHVM_ME(ZMQSocket, connect);
    HHVM_ME(ZMQSocket, disconnect);
    HHVM_ME(ZMQSocket, recv);
    HHVM_ME(ZMQSocket, send);
    HHVM_ME(ZMQSocket, setSockOpt);
    HHVM_ME(ZMQSocket, getSocketType);
    
    #define REGISTER_ZMQ_CONST_INT(name, value)                                \
    Native::registerClassConstant<KindOfInt64>(s_ZMQ.get(), name.get(),        \
                                              (int64_t) value);                \

    REGISTER_ZMQ_CONST_INT(s_SOCKET_PAIR, ZMQ_PAIR);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_PUB, ZMQ_PUB);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_SUB, ZMQ_SUB);
    
    #if ZMQ_VERSION_MAJOR >= 3
      REGISTER_ZMQ_CONST_INT(s_SOCKET_XSUB, ZMQ_XSUB);
      REGISTER_ZMQ_CONST_INT(s_SOCKET_XPUB, ZMQ_XPUB);
    #endif

    REGISTER_ZMQ_CONST_INT(s_SOCKET_REQ, ZMQ_REQ);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_REP, ZMQ_REP);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_XREQ, ZMQ_XREQ);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_XREP, ZMQ_XREP);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_PUSH, ZMQ_PUSH);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_PULL, ZMQ_PULL);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_DEALER, ZMQ_DEALER);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_ROUTER, ZMQ_ROUTER);
    
    #if ZMQ_MAJOR_VERSION >= 4
      REGISTER_ZMQ_CONST_INT(s_SOCKET_STREAM, ZMQ_STREAM);
    #endif

    REGISTER_ZMQ_CONST_INT(s_SOCKET_UPSTREAM, ZMQ_PULL);
    REGISTER_ZMQ_CONST_INT(s_SOCKET_DOWNSTREAM, ZMQ_PUSH);

    #if ZMQ_VERSION_MAJOR == 3 && ZMQ_VERSION_MINOR == 0
      REGISTER_ZMQ_CONST_INT(s_MODE_SNDLABEL, ZMQ_SNDLABEL);
    #endif

    REGISTER_ZMQ_CONST_INT(s_POLL_IN, ZMQ_POLLIN);
    REGISTER_ZMQ_CONST_INT(s_POLL_OUT, ZMQ_POLLOUT);

    REGISTER_ZMQ_CONST_INT(s_MODE_SNDMORE, ZMQ_SNDMORE);
    REGISTER_ZMQ_CONST_INT(s_MODE_NOBLOCK, ZMQ_DONTWAIT);
    REGISTER_ZMQ_CONST_INT(s_MODE_DONTWAIT, ZMQ_DONTWAIT);

    REGISTER_ZMQ_CONST_INT(s_DEVICE_FORWARDER, ZMQ_FORWARDER);
    REGISTER_ZMQ_CONST_INT(s_DEVICE_QUEUE, ZMQ_QUEUE);
    REGISTER_ZMQ_CONST_INT(s_DEVICE_STREAMER, ZMQ_STREAMER);

    REGISTER_ZMQ_CONST_INT(s_ERR_INTERNAL, -99);
    REGISTER_ZMQ_CONST_INT(s_ERR_EAGAIN, EAGAIN);
    REGISTER_ZMQ_CONST_INT(s_ERR_ENOTSUP, ENOTSUP);
    REGISTER_ZMQ_CONST_INT(s_ERR_EFSM, EFSM);
    REGISTER_ZMQ_CONST_INT(s_ERR_ETERM, ETERM);
    
    #if ZMQ_VERSION_MAJOR >= 3
      REGISTER_ZMQ_CONST_INT(s_SOCKOPT_SNDHWM, ZMQ_SNDHWM);
      REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RCVHWM, ZMQ_RCVHWM);
    #else
      REGISTER_ZMQ_CONST_INT(s_SOCKOPT_HWM, ZMQ_HWM);
    #endif

    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_AFFINITY, ZMQ_AFFINITY);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_IDENTITY, ZMQ_IDENTITY);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RATE, ZMQ_RATE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RECOVERY_IVL, ZMQ_RECOVERY_IVL);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_SNDBUF, ZMQ_SNDBUF);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RCVBUF, ZMQ_RCVBUF);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_LINGER, ZMQ_LINGER);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RECONNECT_IVL, ZMQ_RECONNECT_IVL);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RECONNECT_IVL_MAX, ZMQ_RECONNECT_IVL_MAX);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_BACKLOG, ZMQ_BACKLOG);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_MAXMSGSIZE, ZMQ_MAXMSGSIZE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_SUBSCRIBE, ZMQ_SUBSCRIBE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_UNSUBSCRIBE, ZMQ_UNSUBSCRIBE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_TYPE, ZMQ_TYPE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RCVMORE, ZMQ_RCVMORE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_FD, ZMQ_FD);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_EVENTS, ZMQ_EVENTS);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_SNDTIMEO, ZMQ_SNDTIMEO);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_RCVTIMEO, ZMQ_RCVTIMEO);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_IPV4ONLY, ZMQ_IPV4ONLY);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_LAST_ENDPOINT, ZMQ_LAST_ENDPOINT);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_TCP_KEEPALIVE, ZMQ_TCP_KEEPALIVE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_TCP_KEEPALIVE_IDLE, ZMQ_TCP_KEEPALIVE_IDLE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_TCP_KEEPALIVE_CNT, ZMQ_TCP_KEEPALIVE_CNT);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_TCP_KEEPALIVE_INTVL, ZMQ_TCP_KEEPALIVE_INTVL);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_TCP_ACCEPT_FILTER, ZMQ_TCP_ACCEPT_FILTER);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_DELAY_ATTACH_ON_CONNECT, ZMQ_DELAY_ATTACH_ON_CONNECT);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_XPUB_VERBOSE, ZMQ_XPUB_VERBOSE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_ROUTER_RAW, ZMQ_ROUTER_RAW);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_IPV6, ZMQ_IPV6);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_PLAIN_SERVER, ZMQ_PLAIN_SERVER);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_PLAIN_USERNAME, ZMQ_PLAIN_USERNAME);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_PLAIN_PASSWORD, ZMQ_PLAIN_PASSWORD);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_CURVE_SERVER, ZMQ_CURVE_SERVER);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_CURVE_PUBLICKEY, ZMQ_CURVE_PUBLICKEY);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_CURVE_SECRETKEY, ZMQ_CURVE_SECRETKEY);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_CURVE_SERVERKEY, ZMQ_CURVE_SERVERKEY);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_PROBE_ROUTER, ZMQ_PROBE_ROUTER);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_REQ_CORRELATE, ZMQ_REQ_CORRELATE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_REQ_RELAXED, ZMQ_REQ_RELAXED);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_CONFLATE, ZMQ_CONFLATE);
    REGISTER_ZMQ_CONST_INT(s_SOCKOPT_ZAP_DOMAIN, ZMQ_ZAP_DOMAIN);

    loadSystemlib();
  }
} s_zmq_extension;

HHVM_GET_MODULE(zmq)

} // namespace HPHP
