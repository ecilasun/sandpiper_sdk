/* SPDX-License-Identifier: GPL-2.0 */
#include <pcmcia/ss.h>

struct module;
struct cpufreq_freqs;

struct soc_pcmcia_regulator {
	struct regulator	*reg;
	bool			on;
};

struct pcmcia_state {
  unsigned detect: 1,
            ready: 1,
             bvd1: 1,
             bvd2: 1,
           wrprot: 1,
            vs_3v: 1,
            vs_Xv: 1;
};


struct soc_pcmcia_socket {
	struct pcmcia_socket	socket;

	
	unsigned int		nr;
	struct clk		*clk;

	
	const struct pcmcia_low_level *ops;

	unsigned int		status;
	socket_state_t		cs_state;

	unsigned short		spd_io[MAX_IO_WIN];
	unsigned short		spd_mem[MAX_WIN];
	unsigned short		spd_attr[MAX_WIN];

	struct resource		res_skt;
	struct resource		res_io;
	struct resource		res_io_io;
	struct resource		res_mem;
	struct resource		res_attr;

	struct {
		int		gpio;
		struct gpio_desc *desc;
		unsigned int	irq;
		const char	*name;
	} stat[6];
#define SOC_STAT_CD		0	
#define SOC_STAT_BVD1		1	
#define SOC_STAT_BVD2		2	
#define SOC_STAT_RDY		3	
#define SOC_STAT_VS1		4	
#define SOC_STAT_VS2		5	

	struct gpio_desc	*gpio_reset;
	struct gpio_desc	*gpio_bus_enable;
	struct soc_pcmcia_regulator vcc;
	struct soc_pcmcia_regulator vpp;

	unsigned int		irq_state;

#ifdef CONFIG_CPU_FREQ
	struct notifier_block	cpufreq_nb;
#endif
	struct timer_list	poll_timer;
	struct list_head	node;
	void *driver_data;
};


struct pcmcia_low_level {
	struct module *owner;

	
	int first;
	
	int nr;

	int (*hw_init)(struct soc_pcmcia_socket *);
	void (*hw_shutdown)(struct soc_pcmcia_socket *);

	void (*socket_state)(struct soc_pcmcia_socket *, struct pcmcia_state *);
	int (*configure_socket)(struct soc_pcmcia_socket *, const socket_state_t *);

	
	void (*socket_init)(struct soc_pcmcia_socket *);

	
	void (*socket_suspend)(struct soc_pcmcia_socket *);

	
	unsigned int (*get_timing)(struct soc_pcmcia_socket *, unsigned int, unsigned int);
	int (*set_timing)(struct soc_pcmcia_socket *);
	int (*show_timing)(struct soc_pcmcia_socket *, char *);

#ifdef CONFIG_CPU_FREQ
	
	int (*frequency_change)(struct soc_pcmcia_socket *, unsigned long, struct cpufreq_freqs *);
#endif
};



