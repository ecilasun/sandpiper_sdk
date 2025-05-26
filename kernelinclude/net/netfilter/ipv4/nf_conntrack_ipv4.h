/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _NF_CONNTRACK_IPV4_H
#define _NF_CONNTRACK_IPV4_H

extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_tcp;
extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_udp;
extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_icmp;
#ifdef CONFIG_NF_CT_PROTO_DCCP
extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_dccp;
#endif
#ifdef CONFIG_NF_CT_PROTO_SCTP
extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_sctp;
#endif
#ifdef CONFIG_NF_CT_PROTO_UDPLITE
extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_udplite;
#endif
#ifdef CONFIG_NF_CT_PROTO_GRE
extern const struct nf_conntrack_l4proto nf_conntrack_l4proto_gre;
#endif

#endif 
