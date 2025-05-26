/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _XEN_EVENTS_H
#define _XEN_EVENTS_H

#include <linux/interrupt.h>
#include <linux/irq.h>
#ifdef CONFIG_PCI_MSI
#include <linux/msi.h>
#endif

#include <xen/interface/event_channel.h>
#include <asm/xen/hypercall.h>
#include <asm/xen/events.h>

struct xenbus_device;

unsigned xen_evtchn_nr_channels(void);

int bind_evtchn_to_irq(evtchn_port_t evtchn);
int bind_evtchn_to_irq_lateeoi(evtchn_port_t evtchn);
int bind_evtchn_to_irqhandler(evtchn_port_t evtchn,
			      irq_handler_t handler,
			      unsigned long irqflags, const char *devname,
			      void *dev_id);
int bind_evtchn_to_irqhandler_lateeoi(evtchn_port_t evtchn,
			      irq_handler_t handler,
			      unsigned long irqflags, const char *devname,
			      void *dev_id);
int bind_virq_to_irq(unsigned int virq, unsigned int cpu, bool percpu);
int bind_virq_to_irqhandler(unsigned int virq, unsigned int cpu,
			    irq_handler_t handler,
			    unsigned long irqflags, const char *devname,
			    void *dev_id);
int bind_ipi_to_irqhandler(enum ipi_vector ipi,
			   unsigned int cpu,
			   irq_handler_t handler,
			   unsigned long irqflags,
			   const char *devname,
			   void *dev_id);
int bind_interdomain_evtchn_to_irq_lateeoi(struct xenbus_device *dev,
					   evtchn_port_t remote_port);
int bind_interdomain_evtchn_to_irqhandler_lateeoi(struct xenbus_device *dev,
						  evtchn_port_t remote_port,
						  irq_handler_t handler,
						  unsigned long irqflags,
						  const char *devname,
						  void *dev_id);


void unbind_from_irqhandler(unsigned int irq, void *dev_id);


void xen_irq_lateeoi(unsigned int irq, unsigned int eoi_flags);

#define XEN_EOI_FLAG_SPURIOUS	0x00000001

#define XEN_IRQ_PRIORITY_MAX     EVTCHN_FIFO_PRIORITY_MAX
#define XEN_IRQ_PRIORITY_DEFAULT EVTCHN_FIFO_PRIORITY_DEFAULT
#define XEN_IRQ_PRIORITY_MIN     EVTCHN_FIFO_PRIORITY_MIN
int xen_set_irq_priority(unsigned irq, unsigned priority);


int evtchn_make_refcounted(evtchn_port_t evtchn, bool is_static);
int evtchn_get(evtchn_port_t evtchn);
void evtchn_put(evtchn_port_t evtchn);

void xen_send_IPI_one(unsigned int cpu, enum ipi_vector vector);
void rebind_evtchn_irq(evtchn_port_t evtchn, int irq);

static inline void notify_remote_via_evtchn(evtchn_port_t port)
{
	struct evtchn_send send = { .port = port };
	(void)HYPERVISOR_event_channel_op(EVTCHNOP_send, &send);
}

void notify_remote_via_irq(int irq);

void xen_irq_resume(void);


void xen_clear_irq_pending(int irq);
void xen_set_irq_pending(int irq);
bool xen_test_irq_pending(int irq);


void xen_poll_irq(int irq);


void xen_poll_irq_timeout(int irq, u64 timeout);


unsigned int irq_from_evtchn(evtchn_port_t evtchn);
int irq_evtchn_from_virq(unsigned int cpu, unsigned int virq,
			 evtchn_port_t *evtchn);

int xen_set_callback_via(uint64_t via);
int xen_evtchn_do_upcall(void);


int xen_bind_pirq_gsi_to_irq(unsigned gsi,
			     unsigned pirq, int shareable, char *name);

#ifdef CONFIG_PCI_MSI

int xen_allocate_pirq_msi(struct pci_dev *dev, struct msi_desc *msidesc);

int xen_bind_pirq_msi_to_irq(struct pci_dev *dev, struct msi_desc *msidesc,
			     int pirq, int nvec, const char *name, domid_t domid);
#endif


int xen_destroy_irq(int irq);


int xen_irq_from_pirq(unsigned pirq);


int xen_pirq_from_irq(unsigned irq);


int xen_irq_from_gsi(unsigned gsi);


int xen_test_irq_shared(int irq);


void xen_init_IRQ(void);

irqreturn_t xen_debug_interrupt(int irq, void *dev_id);

static inline void xen_evtchn_close(evtchn_port_t port)
{
	struct evtchn_close close;

	close.port = port;
	if (HYPERVISOR_event_channel_op(EVTCHNOP_close, &close) != 0)
		BUG();
}

#endif	
