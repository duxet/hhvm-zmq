#include "hphp/runtime/base/base-includes.h"
#include "hphp/util/string-vsnprintf.h"

#include "zmq.h"
#include "zmq_common.h"

namespace HPHP {
    
const StaticString
  s_code("code"),
  s_message("message"),
  s_ZMQSocketException("ZMQSocketException");
    
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

  result = zmq_send(socket->get(), message.c_str(), message.size() + 1, mode);

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
    
    loadSystemlib();
  }
} s_zmq_extension;

HHVM_GET_MODULE(zmq)

} // namespace HPHP
