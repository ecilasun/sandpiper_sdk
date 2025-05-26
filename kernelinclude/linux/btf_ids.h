/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_BTF_IDS_H
#define _LINUX_BTF_IDS_H

struct btf_id_set {
	u32 cnt;
	u32 ids[];
};

struct btf_id_set8 {
	u32 cnt;
	u32 flags;
	struct {
		u32 id;
		u32 flags;
	} pairs[];
};

#ifdef CONFIG_DEBUG_INFO_BTF

#include <linux/compiler.h> 
#include <linux/compiler_attributes.h> 



#define BTF_IDS_SECTION ".BTF_ids"

#define ____BTF_ID(symbol, word)			\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";       \n"	\
".local " #symbol " ;                          \n"	\
".type  " #symbol ", STT_OBJECT;               \n"	\
".size  " #symbol ", 4;                        \n"	\
#symbol ":                                     \n"	\
".zero 4                                       \n"	\
word							\
".popsection;                                  \n");

#define __BTF_ID(symbol, word) \
	____BTF_ID(symbol, word)

#define __ID(prefix) \
	__PASTE(__PASTE(prefix, __COUNTER__), __LINE__)


#define BTF_ID(prefix, name) \
	__BTF_ID(__ID(__BTF_ID__##prefix##__##name##__), "")

#define ____BTF_ID_FLAGS(prefix, name, flags) \
	__BTF_ID(__ID(__BTF_ID__##prefix##__##name##__), ".long " #flags "\n")
#define __BTF_ID_FLAGS(prefix, name, flags, ...) \
	____BTF_ID_FLAGS(prefix, name, flags)
#define BTF_ID_FLAGS(prefix, name, ...) \
	__BTF_ID_FLAGS(prefix, name, ##__VA_ARGS__, 0)


#define __BTF_ID_LIST(name, scope)			\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";       \n"	\
"." #scope " " #name ";                        \n"	\
#name ":;                                      \n"	\
".popsection;                                  \n");

#define BTF_ID_LIST(name)				\
__BTF_ID_LIST(name, local)				\
extern u32 name[];

#define BTF_ID_LIST_GLOBAL(name, n)			\
__BTF_ID_LIST(name, globl)


#define BTF_ID_LIST_SINGLE(name, prefix, typename)	\
	BTF_ID_LIST(name) \
	BTF_ID(prefix, typename)
#define BTF_ID_LIST_GLOBAL_SINGLE(name, prefix, typename) \
	BTF_ID_LIST_GLOBAL(name, 1)			  \
	BTF_ID(prefix, typename)



#define BTF_ID_UNUSED					\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";       \n"	\
".zero 4                                       \n"	\
".popsection;                                  \n");


#define __BTF_SET_START(name, scope)			\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";       \n"	\
"." #scope " __BTF_ID__set__" #name ";         \n"	\
"__BTF_ID__set__" #name ":;                    \n"	\
".zero 4                                       \n"	\
".popsection;                                  \n");

#define BTF_SET_START(name)				\
__BTF_ID_LIST(name, local)				\
__BTF_SET_START(name, local)

#define BTF_SET_START_GLOBAL(name)			\
__BTF_ID_LIST(name, globl)				\
__BTF_SET_START(name, globl)

#define BTF_SET_END(name)				\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";      \n"	\
".size __BTF_ID__set__" #name ", .-" #name "  \n"	\
".popsection;                                 \n");	\
extern struct btf_id_set name;


#define __BTF_SET8_START(name, scope)			\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";       \n"	\
"." #scope " __BTF_ID__set8__" #name ";        \n"	\
"__BTF_ID__set8__" #name ":;                   \n"	\
".zero 8                                       \n"	\
".popsection;                                  \n");

#define BTF_SET8_START(name)				\
__BTF_ID_LIST(name, local)				\
__BTF_SET8_START(name, local)

#define BTF_SET8_END(name)				\
asm(							\
".pushsection " BTF_IDS_SECTION ",\"a\";      \n"	\
".size __BTF_ID__set8__" #name ", .-" #name "  \n"	\
".popsection;                                 \n");	\
extern struct btf_id_set8 name;

#else

#define BTF_ID_LIST(name) static u32 __maybe_unused name[64];
#define BTF_ID(prefix, name)
#define BTF_ID_FLAGS(prefix, name, ...)
#define BTF_ID_UNUSED
#define BTF_ID_LIST_GLOBAL(name, n) u32 __maybe_unused name[n];
#define BTF_ID_LIST_SINGLE(name, prefix, typename) static u32 __maybe_unused name[1];
#define BTF_ID_LIST_GLOBAL_SINGLE(name, prefix, typename) u32 __maybe_unused name[1];
#define BTF_SET_START(name) static struct btf_id_set __maybe_unused name = { 0 };
#define BTF_SET_START_GLOBAL(name) static struct btf_id_set __maybe_unused name = { 0 };
#define BTF_SET_END(name)
#define BTF_SET8_START(name) static struct btf_id_set8 __maybe_unused name = { 0 };
#define BTF_SET8_END(name)

#endif 

#ifdef CONFIG_NET

#define BTF_SOCK_TYPE_xxx \
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_INET, inet_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_INET_CONN, inet_connection_sock)	\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_INET_REQ, inet_request_sock)	\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_INET_TW, inet_timewait_sock)	\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_REQ, request_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_SOCK, sock)				\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_SOCK_COMMON, sock_common)		\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_TCP, tcp_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_TCP_REQ, tcp_request_sock)		\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_TCP_TW, tcp_timewait_sock)		\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_TCP6, tcp6_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_UDP, udp_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_UDP6, udp6_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_UNIX, unix_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_MPTCP, mptcp_sock)			\
	BTF_SOCK_TYPE(BTF_SOCK_TYPE_SOCKET, socket)

enum {
#define BTF_SOCK_TYPE(name, str) name,
BTF_SOCK_TYPE_xxx
#undef BTF_SOCK_TYPE
MAX_BTF_SOCK_TYPE,
};

extern u32 btf_sock_ids[];
#endif

#define BTF_TRACING_TYPE_xxx	\
	BTF_TRACING_TYPE(BTF_TRACING_TYPE_TASK, task_struct)	\
	BTF_TRACING_TYPE(BTF_TRACING_TYPE_FILE, file)		\
	BTF_TRACING_TYPE(BTF_TRACING_TYPE_VMA, vm_area_struct)

enum {
#define BTF_TRACING_TYPE(name, type) name,
BTF_TRACING_TYPE_xxx
#undef BTF_TRACING_TYPE
MAX_BTF_TRACING_TYPE,
};

extern u32 btf_tracing_ids[];
extern u32 bpf_cgroup_btf_id[];
extern u32 bpf_local_storage_map_btf_id[];
extern u32 btf_bpf_map_id[];

#endif
