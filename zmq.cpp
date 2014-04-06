#include "hphp/runtime/base/base-includes.h"

#include "zmq.h"
#include "zmq_common.h"

namespace HPHP {

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

static Object HHVM_METHOD(ZMQSocket, connect, const String& dsn, bool force) {
  auto socket = getResource<SocketData>(this_, "socket");

  int64_t result = zmq_connect(socket->get(), dsn.c_str());

  return this_;
}

static Object HHVM_METHOD(ZMQSocket, send, const String& message, int64_t mode) {
  auto socket = getResource<SocketData>(this_, "socket");

  int64_t result = zmq_send(socket->get(), message.c_str(), message.size() + 1, mode);

  return this_;
}

static Object HHVM_METHOD(ZMQSocket, setSockOpt, int64_t key, const Variant& value) {
  auto socket = getResource<SocketData>(this_, "socket");

  String sValue = value.toString();
  
  int64_t result = zmq_setsockopt(socket->get(), key, sValue.c_str(), sValue.size() + 1);

  return this_;
}

static class ZMQExtension : public Extension {
 public:
  ZMQExtension() : Extension("zmq") {}
  virtual void moduleInit() {
    HHVM_ME(ZMQContext, __construct);
    
    HHVM_ME(ZMQSocket, __construct);
    HHVM_ME(ZMQSocket, connect);
    HHVM_ME(ZMQSocket, send);
    HHVM_ME(ZMQSocket, setSockOpt);
    
    loadSystemlib();
  }
} s_zmq_extension;

HHVM_GET_MODULE(zmq)

} // namespace HPHP
