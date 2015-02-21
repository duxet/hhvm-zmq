#include "hphp/runtime/base/base-includes.h"
#include "string.h"

#include <zmq.h>

namespace HPHP {

const StaticString
  s_zmqcontext("ZMQContext"),
  s_zmqsocket("ZMQSocket"),
  s_context("__context");

// ContextData

class ContextData : public SweepableResourceData {
public:
  static ContextData *GetPersistent(int64_t io_threads);
  static void SetPersistent(int64_t io_threads, ContextData *context);

private:
  static std::string GetHash(const char *name, int64_t io_threads);
  static ContextData *GetCachedImpl(const char *name, int64_t io_threads);
  static void SetCachedImpl(const char *name, int64_t io_threads, ContextData *context);

public:
  ContextData(int64_t io_threads);
  ~ContextData();

  CLASSNAME_IS("Context")

  // overriding ResourceData
  virtual const String& o_getClassNameHook() const { return classnameof(); }
  virtual bool isInvalid() const { return m_context == nullptr; }

  void *get() { return m_context; }

private:
  void *m_context;

};

ContextData *get_context(Object obj);

// SocketData

class SocketData : public ResourceData {
 public:
  DECLARE_RESOURCE_ALLOCATION_NO_SWEEP(SocketData)
 
  SocketData(void *context, int64_t type) {
    m_socket = zmq_socket(context, type);
  }
  
  ~SocketData() {
    close();
  }        
          
  CLASSNAME_IS("SocketData");
  // overriding ResourceData
  const String& o_getClassNameHook() const { return classnameof(); }

  void close() {
    if (!isValid())
        return;
    
    zmq_close(m_socket);
    m_socket = nullptr;
  }

  bool isValid() { return m_socket != nullptr; }
  
  void *get() { return m_socket; }

 private:
  void *m_socket;
};

} // namespace HPHP
