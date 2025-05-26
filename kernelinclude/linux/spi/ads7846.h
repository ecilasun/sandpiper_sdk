/* SPDX-License-Identifier: GPL-2.0 */


struct ads7846_platform_data {
	u16	model;			
	u16	vref_delay_usecs;	
	u16	vref_mv;		
	bool	keep_vref_on;		
	bool	swap_xy;		

	
	u16	settle_delay_usecs;

	
	u16	penirq_recheck_delay_usecs;

	u16	x_plate_ohms;
	u16	y_plate_ohms;

	u16	x_min, x_max;
	u16	y_min, y_max;
	u16	pressure_min, pressure_max;

	u16	debounce_max;		
	u16	debounce_tol;		
	u16	debounce_rep;		
	int	gpio_pendown_debounce;	
	int	(*get_pendown_state)(void);
	void	(*wait_for_sync)(void);
	bool	wakeup;
	unsigned long irq_flags;
};

