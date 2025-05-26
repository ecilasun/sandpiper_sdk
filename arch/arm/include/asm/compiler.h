/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_ARM_COMPILER_H
#define __ASM_ARM_COMPILER_H


#define __asmeq(x, y)				\
	".ifnc " x "," y "; "			\
	  ".ifnc " x y ",fpr11; " 		\
	    ".ifnc " x y ",r11fp; "		\
	      ".ifnc " x y ",ipr12; " 		\
	        ".ifnc " x y ",r12ip; "		\
	          ".err; "			\
	        ".endif; "			\
	      ".endif; "			\
	    ".endif; "				\
	  ".endif; "				\
	".endif\n\t"


#endif 
