#undef TRACE_SYSTEM
#define TRACE_SYSTEM iscsi

#if !defined(_TRACE_ISCSI_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_ISCSI_H

#include <linux/tracepoint.h>


#define ISCSI_MSG_MAX	256


void iscsi_dbg_trace(void (*trace)(struct device *dev, struct va_format *),
		     struct device *dev, const char *fmt, ...);


DECLARE_EVENT_CLASS(iscsi_log_msg,

	TP_PROTO(struct device *dev, struct va_format *vaf),

	TP_ARGS(dev, vaf),

	TP_STRUCT__entry(
		__string(dname, 	dev_name(dev)		)
		__vstring(msg,		vaf->fmt, vaf->va)
	),

	TP_fast_assign(
		__assign_str(dname, dev_name(dev));
		__assign_vstr(msg, vaf->fmt, vaf->va);
	),

	TP_printk("%s: %s",__get_str(dname),  __get_str(msg)
	)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_conn,
	TP_PROTO(struct device *dev, struct va_format *vaf),

	TP_ARGS(dev, vaf)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_session,
	TP_PROTO(struct device *dev, struct va_format *vaf),

	TP_ARGS(dev, vaf)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_eh,
        TP_PROTO(struct device *dev, struct va_format *vaf),

        TP_ARGS(dev, vaf)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_tcp,
        TP_PROTO(struct device *dev, struct va_format *vaf),

        TP_ARGS(dev, vaf)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_sw_tcp,
	TP_PROTO(struct device *dev, struct va_format *vaf),

	TP_ARGS(dev, vaf)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_trans_session,
	TP_PROTO(struct device *dev, struct va_format *vaf),

	TP_ARGS(dev, vaf)
);


DEFINE_EVENT(iscsi_log_msg, iscsi_dbg_trans_conn,
	TP_PROTO(struct device *dev, struct va_format *vaf),

	TP_ARGS(dev, vaf)
);

#endif 


#include <trace/define_trace.h>
