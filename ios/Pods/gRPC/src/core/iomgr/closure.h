/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef GRPC_INTERNAL_CORE_IOMGR_CLOSURE_H
#define GRPC_INTERNAL_CORE_IOMGR_CLOSURE_H

#include <grpc/support/port_platform.h>

struct grpc_closure;
typedef struct grpc_closure grpc_closure;

/* forward declaration for exec_ctx.h */
struct grpc_exec_ctx;
typedef struct grpc_exec_ctx grpc_exec_ctx;

typedef struct grpc_closure_list {
  grpc_closure *head;
  grpc_closure *tail;
} grpc_closure_list;

/** gRPC Callback definition.
 *
 * \param arg Arbitrary input.
 * \param success An indication on the state of the iomgr. On false, cleanup
 * actions should be taken (eg, shutdown). */
typedef void (*grpc_iomgr_cb_func)(grpc_exec_ctx *exec_ctx, void *arg,
                                   int success);

/** A closure over a grpc_iomgr_cb_func. */
struct grpc_closure {
  /** Bound callback. */
  grpc_iomgr_cb_func cb;

  /** Arguments to be passed to "cb". */
  void *cb_arg;

  /** Once enqueued, contains in the lower bit the success of the closure,
      and in the upper bits the pointer to the next closure in the list.
      Before enqueing for execution, this is usable for scratch data. */
  gpr_uintptr final_data;
};

/** Initializes \a closure with \a cb and \a cb_arg. */
void grpc_closure_init(grpc_closure *closure, grpc_iomgr_cb_func cb,
                       void *cb_arg);

/* Create a heap allocated closure: try to avoid except for very rare events */
grpc_closure *grpc_closure_create(grpc_iomgr_cb_func cb, void *cb_arg);

#define GRPC_CLOSURE_LIST_INIT \
  { NULL, NULL }

/** add \a closure to the end of \a list and set \a closure's success to \a
 * success */
void grpc_closure_list_add(grpc_closure_list *list, grpc_closure *closure,
                           int success);

/** append all closures from \a src to \a dst and empty \a src. */
void grpc_closure_list_move(grpc_closure_list *src, grpc_closure_list *dst);

/** return whether \a list is empty. */
int grpc_closure_list_empty(grpc_closure_list list);

/** return the next pointer for a queued closure list */
grpc_closure *grpc_closure_next(grpc_closure *closure);

#endif /* GRPC_INTERNAL_CORE_IOMGR_CLOSURE_H */
