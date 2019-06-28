/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2019 Marvell International Ltd.
 */

#ifndef __OTX2_EVDEV_H__
#define __OTX2_EVDEV_H__

#include <rte_eventdev.h>

#include "otx2_common.h"
#include "otx2_dev.h"
#include "otx2_mempool.h"

#define EVENTDEV_NAME_OCTEONTX2_PMD otx2_eventdev

#define sso_func_trace otx2_sso_dbg

#define OTX2_SSO_MAX_VHGRP                  RTE_EVENT_MAX_QUEUES_PER_DEV
#define OTX2_SSO_MAX_VHWS                   (UINT8_MAX)
#define OTX2_SSO_FC_NAME                    "otx2_evdev_xaq_fc"
#define OTX2_SSO_XAQ_SLACK                  (8)
#define OTX2_SSO_XAQ_CACHE_CNT              (0x7)

/* SSO LF register offsets (BAR2) */
#define SSO_LF_GGRP_OP_ADD_WORK0            (0x0ull)
#define SSO_LF_GGRP_OP_ADD_WORK1            (0x8ull)

#define SSO_LF_GGRP_QCTL                    (0x20ull)
#define SSO_LF_GGRP_EXE_DIS                 (0x80ull)
#define SSO_LF_GGRP_INT                     (0x100ull)
#define SSO_LF_GGRP_INT_W1S                 (0x108ull)
#define SSO_LF_GGRP_INT_ENA_W1S             (0x110ull)
#define SSO_LF_GGRP_INT_ENA_W1C             (0x118ull)
#define SSO_LF_GGRP_INT_THR                 (0x140ull)
#define SSO_LF_GGRP_INT_CNT                 (0x180ull)
#define SSO_LF_GGRP_XAQ_CNT                 (0x1b0ull)
#define SSO_LF_GGRP_AQ_CNT                  (0x1c0ull)
#define SSO_LF_GGRP_AQ_THR                  (0x1e0ull)
#define SSO_LF_GGRP_MISC_CNT                (0x200ull)

/* SSOW LF register offsets (BAR2) */
#define SSOW_LF_GWS_LINKS                   (0x10ull)
#define SSOW_LF_GWS_PENDWQP                 (0x40ull)
#define SSOW_LF_GWS_PENDSTATE               (0x50ull)
#define SSOW_LF_GWS_NW_TIM                  (0x70ull)
#define SSOW_LF_GWS_GRPMSK_CHG              (0x80ull)
#define SSOW_LF_GWS_INT                     (0x100ull)
#define SSOW_LF_GWS_INT_W1S                 (0x108ull)
#define SSOW_LF_GWS_INT_ENA_W1S             (0x110ull)
#define SSOW_LF_GWS_INT_ENA_W1C             (0x118ull)
#define SSOW_LF_GWS_TAG                     (0x200ull)
#define SSOW_LF_GWS_WQP                     (0x210ull)
#define SSOW_LF_GWS_SWTP                    (0x220ull)
#define SSOW_LF_GWS_PENDTAG                 (0x230ull)
#define SSOW_LF_GWS_OP_ALLOC_WE             (0x400ull)
#define SSOW_LF_GWS_OP_GET_WORK             (0x600ull)
#define SSOW_LF_GWS_OP_SWTAG_FLUSH          (0x800ull)
#define SSOW_LF_GWS_OP_SWTAG_UNTAG          (0x810ull)
#define SSOW_LF_GWS_OP_SWTP_CLR             (0x820ull)
#define SSOW_LF_GWS_OP_UPD_WQP_GRP0         (0x830ull)
#define SSOW_LF_GWS_OP_UPD_WQP_GRP1         (0x838ull)
#define SSOW_LF_GWS_OP_DESCHED              (0x880ull)
#define SSOW_LF_GWS_OP_DESCHED_NOSCH        (0x8c0ull)
#define SSOW_LF_GWS_OP_SWTAG_DESCHED        (0x980ull)
#define SSOW_LF_GWS_OP_SWTAG_NOSCHED        (0x9c0ull)
#define SSOW_LF_GWS_OP_CLR_NSCHED0          (0xa00ull)
#define SSOW_LF_GWS_OP_CLR_NSCHED1          (0xa08ull)
#define SSOW_LF_GWS_OP_SWTP_SET             (0xc00ull)
#define SSOW_LF_GWS_OP_SWTAG_NORM           (0xc10ull)
#define SSOW_LF_GWS_OP_SWTAG_FULL0          (0xc20ull)
#define SSOW_LF_GWS_OP_SWTAG_FULL1          (0xc28ull)
#define SSOW_LF_GWS_OP_GWC_INVAL            (0xe00ull)

#define OTX2_SSOW_GET_BASE_ADDR(_GW)        ((_GW) - SSOW_LF_GWS_OP_GET_WORK)

#define NSEC2USEC(__ns)			((__ns) / 1E3)
#define USEC2NSEC(__us)                 ((__us) * 1E3)
#define NSEC2TICK(__ns, __freq)		(((__ns) * (__freq)) / 1E9)

enum otx2_sso_lf_type {
	SSO_LF_GGRP,
	SSO_LF_GWS
};

union otx2_sso_event {
	uint64_t get_work0;
	struct {
		uint32_t flow_id:20;
		uint32_t sub_event_type:8;
		uint32_t event_type:4;
		uint8_t op:2;
		uint8_t rsvd:4;
		uint8_t sched_type:2;
		uint8_t queue_id;
		uint8_t priority;
		uint8_t impl_opaque;
	};
} __rte_aligned(64);

enum {
	SSO_SYNC_ORDERED,
	SSO_SYNC_ATOMIC,
	SSO_SYNC_UNTAGGED,
	SSO_SYNC_EMPTY
};

struct otx2_sso_evdev {
	OTX2_DEV; /* Base class */
	uint8_t max_event_queues;
	uint8_t max_event_ports;
	uint8_t is_timeout_deq;
	uint8_t nb_event_queues;
	uint8_t nb_event_ports;
	uint8_t configured;
	uint32_t deq_tmo_ns;
	uint32_t min_dequeue_timeout_ns;
	uint32_t max_dequeue_timeout_ns;
	int32_t max_num_events;
	uint64_t *fc_mem;
	uint64_t xaq_lmt;
	uint64_t nb_xaq_cfg;
	rte_iova_t fc_iova;
	struct rte_mempool *xaq_pool;
	uint8_t dual_ws;
	/* Dev args */
	uint32_t xae_cnt;
	/* HW const */
	uint32_t xae_waes;
	uint32_t xaq_buf_size;
	uint32_t iue;
	/* MSIX offsets */
	uint16_t sso_msixoff[OTX2_SSO_MAX_VHGRP];
	uint16_t ssow_msixoff[OTX2_SSO_MAX_VHWS];
} __rte_cache_aligned;

#define OTX2_SSOGWS_OPS \
	/* WS ops */			\
	uintptr_t getwrk_op;		\
	uintptr_t tag_op;		\
	uintptr_t wqp_op;		\
	uintptr_t swtp_op;		\
	uintptr_t swtag_norm_op;	\
	uintptr_t swtag_desched_op;	\
	uint8_t cur_tt;			\
	uint8_t cur_grp

/* Event port aka GWS */
struct otx2_ssogws {
	/* Get Work Fastpath data */
	OTX2_SSOGWS_OPS;
	uint8_t swtag_req;
	uint8_t port;
	/* Add Work Fastpath data */
	uint64_t xaq_lmt __rte_cache_aligned;
	uint64_t *fc_mem;
	uintptr_t grps_base[OTX2_SSO_MAX_VHGRP];
} __rte_cache_aligned;

struct otx2_ssogws_state {
	OTX2_SSOGWS_OPS;
};

struct otx2_ssogws_dual {
	/* Get Work Fastpath data */
	struct otx2_ssogws_state ws_state[2]; /* Ping and Pong */
	uint8_t swtag_req;
	uint8_t vws; /* Ping pong bit */
	uint8_t port;
	/* Add Work Fastpath data */
	uint64_t xaq_lmt __rte_cache_aligned;
	uint64_t *fc_mem;
	uintptr_t grps_base[OTX2_SSO_MAX_VHGRP];
} __rte_cache_aligned;

static inline struct otx2_sso_evdev *
sso_pmd_priv(const struct rte_eventdev *event_dev)
{
	return event_dev->data->dev_private;
}

static inline int
parse_kvargs_value(const char *key, const char *value, void *opaque)
{
	RTE_SET_USED(key);

	*(uint32_t *)opaque = (uint32_t)atoi(value);
	return 0;
}

/* Single WS API's */
uint16_t otx2_ssogws_enq(void *port, const struct rte_event *ev);
uint16_t otx2_ssogws_enq_burst(void *port, const struct rte_event ev[],
			       uint16_t nb_events);
uint16_t otx2_ssogws_enq_new_burst(void *port, const struct rte_event ev[],
				   uint16_t nb_events);
uint16_t otx2_ssogws_enq_fwd_burst(void *port, const struct rte_event ev[],
				   uint16_t nb_events);

uint16_t otx2_ssogws_deq(void *port, struct rte_event *ev,
			 uint64_t timeout_ticks);
uint16_t otx2_ssogws_deq_burst(void *port, struct rte_event ev[],
			       uint16_t nb_events, uint64_t timeout_ticks);
uint16_t otx2_ssogws_deq_timeout(void *port, struct rte_event *ev,
				 uint64_t timeout_ticks);
uint16_t otx2_ssogws_deq_timeout_burst(void *port, struct rte_event ev[],
				       uint16_t nb_events,
				       uint64_t timeout_ticks);
/* Dual WS API's */
uint16_t otx2_ssogws_dual_enq(void *port, const struct rte_event *ev);
uint16_t otx2_ssogws_dual_enq_burst(void *port, const struct rte_event ev[],
				    uint16_t nb_events);
uint16_t otx2_ssogws_dual_enq_new_burst(void *port, const struct rte_event ev[],
					uint16_t nb_events);
uint16_t otx2_ssogws_dual_enq_fwd_burst(void *port, const struct rte_event ev[],
					uint16_t nb_events);

uint16_t otx2_ssogws_dual_deq(void *port, struct rte_event *ev,
			      uint64_t timeout_ticks);
uint16_t otx2_ssogws_dual_deq_burst(void *port, struct rte_event ev[],
				    uint16_t nb_events, uint64_t timeout_ticks);
uint16_t otx2_ssogws_dual_deq_timeout(void *port, struct rte_event *ev,
				      uint64_t timeout_ticks);
uint16_t otx2_ssogws_dual_deq_timeout_burst(void *port, struct rte_event ev[],
					    uint16_t nb_events,
					    uint64_t timeout_ticks);
/* Init and Fini API's */
int otx2_sso_init(struct rte_eventdev *event_dev);
int otx2_sso_fini(struct rte_eventdev *event_dev);
/* IRQ handlers */
int sso_register_irqs(const struct rte_eventdev *event_dev);
void sso_unregister_irqs(const struct rte_eventdev *event_dev);

#endif /* __OTX2_EVDEV_H__ */