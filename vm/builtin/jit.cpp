#include "builtin/block_environment.hpp"
#include "builtin/jit.hpp"
#include "builtin/list.hpp"
#include "configuration.hpp"

#include "ontology.hpp"

#include "jit/llvm/state.hpp"

namespace rubinius {
  void JIT::init(STATE) {
    Module* jit = state->new_object<JIT>(G(module));
    jit->setup(state, "JIT", G(rubinius));
    GO(jit).set(jit);

    Class* cls = ontology::new_class_under(state, "CompileRequest", G(jit));
    G(jit)->compile_class(state, cls);

    G(jit)->compile_list(state, List::create(state));
  }

  Object* JIT::compile(STATE, Object* object, CompiledCode* code,
                       Object* block_environment, CallFrame* call_frame)
  {
    if(!CBOOL(enabled())) return cFalse;

    GCTokenImpl gct;
    BlockEnvironment* block_env = try_as<BlockEnvironment>(block_environment);
    if(!block_env) block_env = nil<BlockEnvironment>();

    LLVMState* ls = state->shared().llvm_state;
    ls->compile(state, gct, code, call_frame, object->direct_class(state),
        block_env, !block_env->nil_p());

    return cTrue;
  }

  Object* JIT::compile_threshold(STATE) {
    return Integer::from(state, state->shared().config.jit_threshold_compile);
  }

  Object* JIT::sync_set(STATE, Object* flag) {
    state->shared().config.jit_sync.set(CBOOL(flag));
    return sync_get(state);
  }

  Object* JIT::sync_get(STATE) {
    return RBOOL(state->shared().config.jit_sync);
  }

  Object* JIT::enable(STATE) {
    if(!CBOOL(enabled())) return cFalse;

    state->shared().llvm_state->enable(state);
    enabled(state, cTrue);

    return cTrue;
  }

  Object* JIT::compile_soon(STATE, CompiledCode* code, CallFrame* call_frame,
      Class* receiver_class, BlockEnvironment* block_env, bool is_block)
  {
    if(!CBOOL(enabled())) return cFalse;

    GCTokenImpl gct;

    LLVMState* ls = state->shared().llvm_state;
    ls->compile_soon(state, gct, code, call_frame, receiver_class, block_env, is_block);

    return cTrue;
  }

  Object* JIT::compile_callframe(STATE, CompiledCode* code, CallFrame* call_frame,
      int primitive)
  {
    if(!CBOOL(enabled())) return cFalse;

    GCTokenImpl gct;

    LLVMState* ls = state->shared().llvm_state;
    ls->compile_callframe(state, gct, code, call_frame, primitive);

    return cTrue;
  }

  Object* JIT::start_method_update(STATE) {
    if(!CBOOL(enabled())) return cFalse;

    LLVMState* ls = state->shared().llvm_state;
    ls->start_method_update();

    return cTrue;
  }

  Object* JIT::end_method_update(STATE) {
    if(!CBOOL(enabled())) return cFalse;

    LLVMState* ls = state->shared().llvm_state;
    ls->end_method_update();

    return cTrue;
  }

  JITCompileRequest* JITCompileRequest::create(STATE, CompiledCode* code,
      Class* receiver_class, int hits, BlockEnvironment* block_env, bool is_block)
  {
    JITCompileRequest* request = state->new_object<JITCompileRequest>(
        G(jit)->compile_class());
    request->method(state, code);
    request->receiver_class(state, receiver_class);
    request->block_env(state, block_env);
    request->hits_ = hits;
    request->is_block_ = is_block;
    request->waiter_ = NULL;

    return request;
  }
}
