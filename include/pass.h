#ifndef __PASS_H__
#define __PASS_H__

#include "map.h"
#include "cast.h"

#define ADD_VAR(x) arg ## x
#define ADD_ARG(type, n) type ADD_VAR(n)
#define ADD_ARGS(...) MAPIDX_LIST(ADD_ARG, __VA_ARGS__)

#define MAP_ARG(type, n) ADD_VAR(n)
#define MAP_ARGS(...) MAPIDX_LIST(MAP_ARG, __VA_ARGS__)

#define FIRST_VAR(...) CALLARG(ADD_VAR, CNT_ARGS(__VA_ARGS__))

#define PASS_DEC(type, name, ...)                                                 \
  typedef type name ## _ret;                                                      \
  name ## _ret name(Node, ## __VA_ARGS__);                                        \
  extern name ## _ret (*name ## fp[])(Node, ## __VA_ARGS__)

#define PASS_PROTOTYPE(T, name, ...)                                              \
  name ## _ret name ## _ ## T ## Node(Node, ##__VA_ARGS__);                              \
  name ## _ret _ ## name ## _ ## T ## Node(T ## Node, ##__VA_ARGS__);

#define PASS_INIT_FPTR(T, name, ...)                                              \
  [T      ]  = name ## _ ## T ## Node,                                            \


#define PASS_DEF(name, ...)                                                       \
  MAPCLS(PASS_PROTOTYPE, (name, ## __VA_ARGS__), , NodeTypes)                     \
  name ## _ret name(ADD_ARGS(Node, ## __VA_ARGS__)){                              \
    return FIRST_VAR(Node, ## __VA_ARGS__) ? \
      name ## fp[nd(FIRST_VAR(Node, ## __VA_ARGS__))->nt]                         \
        (MAP_ARGS(Node, ## __VA_ARGS__)) :                                        \
      name ## _Node(MAP_ARGS(Node, ## __VA_ARGS__)); \
  }                                           \
  name ## _ret (*name ## fp[])(Node, ## __VA_ARGS__)  = {                         \
  MAPCLS(PASS_INIT_FPTR, (name, ## __VA_ARGS__), NodeTypes)                       \
  }

#define _PASS_HANDLE(DECT, DRPT, name, node_type, var, ...) \
  name ## _ret _ ## name ## _ ## node_type \
          (DECT(node_type , var, ## __VA_ARGS__)); \
  name ## _ret name ## _ ## node_type \
          (DECT(Node, var, ## __VA_ARGS__)) \
  { return _ ## name ## _ ## node_type(\
    DRPT(Node, (node_type)var, ##__VA_ARGS__)); }\
  name ## _ret _ ## name ## _ ## node_type \
          (DECT(node_type , var, ## __VA_ARGS__))

#define MPASS_HANDLE(name, node_type, var, ...) \
  _PASS_HANDLE(UE_DECL_TYPE, UE_DROP_TYPE, name, node_type, var, ##__VA_ARGS__)

#define PASS_HANDLE(name, node_type, var, ...) \
  _PASS_HANDLE(DECL_TYPE, DROP_TYPE, name, node_type, var, ##__VA_ARGS__)


#include "ast.h"

#endif
