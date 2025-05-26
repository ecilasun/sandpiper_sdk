/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef LINUX_CEC_NOTIFIER_H
#define LINUX_CEC_NOTIFIER_H

#include <linux/err.h>
#include <media/cec.h>

struct device;
struct edid;
struct cec_adapter;
struct cec_notifier;

#if IS_REACHABLE(CONFIG_CEC_CORE) && IS_ENABLED(CONFIG_CEC_NOTIFIER)


struct cec_notifier *
cec_notifier_conn_register(struct device *hdmi_dev, const char *port_name,
			   const struct cec_connector_info *conn_info);


void cec_notifier_conn_unregister(struct cec_notifier *n);


struct cec_notifier *
cec_notifier_cec_adap_register(struct device *hdmi_dev, const char *port_name,
			       struct cec_adapter *adap);


void cec_notifier_cec_adap_unregister(struct cec_notifier *n,
				      struct cec_adapter *adap);


void cec_notifier_set_phys_addr(struct cec_notifier *n, u16 pa);


void cec_notifier_set_phys_addr_from_edid(struct cec_notifier *n,
					  const struct edid *edid);


struct device *cec_notifier_parse_hdmi_phandle(struct device *dev);

#else

static inline struct cec_notifier *
cec_notifier_conn_register(struct device *hdmi_dev, const char *port_name,
			   const struct cec_connector_info *conn_info)
{
	
	return (struct cec_notifier *)0xdeadfeed;
}

static inline void cec_notifier_conn_unregister(struct cec_notifier *n)
{
}

static inline struct cec_notifier *
cec_notifier_cec_adap_register(struct device *hdmi_dev, const char *port_name,
			       struct cec_adapter *adap)
{
	
	return (struct cec_notifier *)0xdeadfeed;
}

static inline void cec_notifier_cec_adap_unregister(struct cec_notifier *n,
						    struct cec_adapter *adap)
{
}

static inline void cec_notifier_set_phys_addr(struct cec_notifier *n, u16 pa)
{
}

static inline void cec_notifier_set_phys_addr_from_edid(struct cec_notifier *n,
							const struct edid *edid)
{
}

static inline struct device *cec_notifier_parse_hdmi_phandle(struct device *dev)
{
	return ERR_PTR(-ENODEV);
}

#endif


static inline void cec_notifier_phys_addr_invalidate(struct cec_notifier *n)
{
	cec_notifier_set_phys_addr(n, CEC_PHYS_ADDR_INVALID);
}

#endif
