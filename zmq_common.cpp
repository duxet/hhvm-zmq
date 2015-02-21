#include "zmq_common.h"

namespace HPHP {

// Context    
    
Resource get_context_resource(Object obj) {
  auto res = obj->o_realProp(s_context, ObjectData::RealPropUnchecked, s_zmqcontext.get());

  if (!res || !res->isResource()) {
    return null_resource;
  }

  return res->toResource();
}

ContextData *get_context(Object obj) {
  auto res = get_context_resource(obj);

  return res.getTyped<ContextData>(true, false);
}

ContextData *ContextData::GetPersistent(int64_t io_threads) {
  return GetCachedImpl("zmq::persistent_contexts", io_threads);
}

void ContextData::SetPersistent(int64_t io_threads, ContextData *context) {
  SetCachedImpl("zmq::persistent_contexts", io_threads, context);
}
std::string ContextData::GetHash(const char *name, int64_t io_threads) {
  char buf[1024];
  snprintf(buf, sizeof(buf), "%s:%ld",
           name, io_threads);
  return std::string(buf);
}


namespace {
  thread_local std::unordered_map<std::string,
                                  ContextData *> s_connections;
}

ContextData *ContextData::GetCachedImpl(const char *name, int64_t io_threads) {
  auto key = GetHash(name, io_threads);
  return s_connections[key];
}

void ContextData::SetCachedImpl(const char *name, int64_t io_threads, ContextData *context) {
  auto key = GetHash(name, io_threads);
  s_connections[key] = context;
}

ContextData::ContextData(int64_t io_threads) {
  m_context = zmq_ctx_new();
  
  zmq_ctx_set(m_context, ZMQ_IO_THREADS, io_threads);
}

ContextData::~ContextData() {
  if (m_context != nullptr) {
    zmq_ctx_destroy(m_context);
  }
}

} // namespace HPHP
