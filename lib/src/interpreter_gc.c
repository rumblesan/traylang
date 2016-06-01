#include "dbg.h"

#include "interpreter_gc.h"

#include "interpreter.h"
#include "interpreter_funcs.h"
#include "interpreter_stackframe.h"
#include "object.h"
#include "object_funcs.h"

#include "bclib/list.h"
#include "bclib/stack.h"
#include "bclib/hashmap.h"
#include "bclib/bstrlib.h"

void interpreter_gc(Interpreter *interpreter) {
    debug("Running garbage collection");
    int ocount = list_count(interpreter->objects);
    interpreter_gc_mark(interpreter);
    interpreter_gc_sweep(interpreter);
    int new_max = list_count(interpreter->objects) * 2;
    if (new_max < INITIAL_GC_THRESHOLD) {
        new_max = INITIAL_GC_THRESHOLD;
    }
    interpreter->max_objects = new_max;
    debug("%d objects reduced to %d",
        ocount,
        list_count(interpreter->objects)
    );
}

int mark_variable(HashmapNode *node) {
    Object *obj = node->data;
    check(obj != NULL, "Attempted to mark NULL Variable");
    object_mark(obj);
    return 0;
error:
    return 0;
}

int mark_scopes(Stack *scopes) {
    StackFrame *frame;
    STACK_FOREACH(scopes, el) {
        frame = el->value;
        check(frame != NULL, "Attempted to mark NULL StackFrame");
        hashmap_traverse(
            frame->variables,
            mark_variable
        );
    }
    return 0;
error:
    return 0;
}

void interpreter_gc_mark(Interpreter *interpreter) {
    debug("Marking stack objects");
    Object *obj;
    STACK_FOREACH(interpreter->call_stack, el) {
        obj = el->value;
        check(obj != NULL, "Attempted to mark NULL Object");
        object_mark(obj);
    }
    mark_scopes(interpreter->scopes);
    debug("Marking globals objects");
    hashmap_traverse(interpreter->globals, mark_variable);
    return;
error:
    interpreter_error(interpreter, bfromcstr("Error whilst marking objects"));
    return;
}

void interpreter_gc_sweep(Interpreter *interpreter) {
    debug("Sweeping");
    List *objects = interpreter->objects;
    if (list_count(objects) < 1) {
        return;
    }
    ListNode *node = objects->first;
    check(node != NULL, "Attempted to mark NULL Object");
    ListNode *next_node;
    Object *obj;
    while(node) {
        next_node = node->next;
        obj = node->value;
        check(obj != NULL, "Attempted to sweep NULL Object");

        if (obj->marked == 0) {
            if (node == objects->first) {
                objects->first = node->next;
            }
            if (node == objects->last) {
                objects->last = node->prev;
            }
            if (node->prev) {
                node->prev->next = node->next;
            }
            if (node->next) {
                node->next->prev = node->prev;
            }
            object_destroy(obj);
            free(node);
            objects->length -= 1;
        } else {
            obj->marked = 0;
        }

        node = next_node;
    }
    return;
error:
    interpreter_error(interpreter, bfromcstr("Error whilst collecting garbage"));
    return;
}

