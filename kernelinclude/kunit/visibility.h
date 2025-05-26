/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _KUNIT_VISIBILITY_H
#define _KUNIT_VISIBILITY_H

#if IS_ENABLED(CONFIG_KUNIT)
    
    #define VISIBLE_IF_KUNIT
    
    #define EXPORT_SYMBOL_IF_KUNIT(symbol) EXPORT_SYMBOL_NS(symbol, \
	    EXPORTED_FOR_KUNIT_TESTING)
#else
    #define VISIBLE_IF_KUNIT static
    #define EXPORT_SYMBOL_IF_KUNIT(symbol)
#endif

#endif 
