#include "builtin_list.hpp"

namespace rubinius {

  void init(STATE) {
    Class* cls;
    cls = state->globals.list = state->new_class("List", List::fields);
    state->globals.list_node = state->new_class("Node", state->globals.object,
                                                List::Node::fields, cls);
  }

  List* List::create(STATE) {
    List* list = (List*)state->new_object(state->globals.list);
    list->count = Object::i2n(0);

    return list;
  }

  void List::append(STATE, OBJECT obj) {
    List::Node* node = (List::Node*)state->new_object(state->globals.list_node);
    SET(node, object, obj);
    List::Node* cur_last = last;

    if(!cur_last->nil_p()) {
      SET(cur_last, next, node);
    }

    SET(this, last, node);

    if(first->nil_p()) {
      SET(this, first, node);
    }

    count = Object::i2n(state, count->n2i() + 1);
  }

  OBJECT List::shift(STATE) {
    if(empty_p()) return Qnil;

    count = Object::i2n(state, count->n2i() - 1);
    List::Node* n = first;
    SET(this, first, first->next);

    if(last == n) {
      last = (Node*)Qnil;
    }

    return n->object;
  }

  size_t List::remove(STATE, OBJECT obj) {
    if(empty_p()) return 0;

    size_t deleted = 0, counted = 0;

    Node* node = first;
    Node* lst =  (Node*)Qnil;
    Node* nxt =  (Node*)Qnil;

    while(!node->nil_p()) {
      if(node->object == obj) {
        deleted++;
        if(lst->nil_p()) {
          SET(this, first, nxt);
        } else {
          SET(lst, next, nxt);
        }

        if(last == node) {
          SET(this, last, lst);
        }
      } else {
        counted++;
      }

      lst = node;
      node = nxt;
    }

    count = Object::i2n(state, counted);

    return deleted;
  }
};

