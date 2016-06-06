
#ifndef __ggn_marshal_MARSHAL_H__
#define __ggn_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* NONE:NONE (./ggn-marshal.list:1) */
#define ggn_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID
#define ggn_marshal_NONE__NONE	ggn_marshal_VOID__VOID

/* NONE:BOOLEAN (./ggn-marshal.list:2) */
#define ggn_marshal_VOID__BOOLEAN	g_cclosure_marshal_VOID__BOOLEAN
#define ggn_marshal_NONE__BOOLEAN	ggn_marshal_VOID__BOOLEAN

/* NONE:STRING,STRING,OBJECT (./ggn-marshal.list:3) */
extern void ggn_marshal_VOID__STRING_STRING_OBJECT (GClosure     *closure,
                                                    GValue       *return_value,
                                                    guint         n_param_values,
                                                    const GValue *param_values,
                                                    gpointer      invocation_hint,
                                                    gpointer      marshal_data);
#define ggn_marshal_NONE__STRING_STRING_OBJECT	ggn_marshal_VOID__STRING_STRING_OBJECT

/* NONE:STRING,STRING (./ggn-marshal.list:4) */
extern void ggn_marshal_VOID__STRING_STRING (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);
#define ggn_marshal_NONE__STRING_STRING	ggn_marshal_VOID__STRING_STRING

/* NONE:STRING,STRING,STRING (./ggn-marshal.list:5) */
extern void ggn_marshal_VOID__STRING_STRING_STRING (GClosure     *closure,
                                                    GValue       *return_value,
                                                    guint         n_param_values,
                                                    const GValue *param_values,
                                                    gpointer      invocation_hint,
                                                    gpointer      marshal_data);
#define ggn_marshal_NONE__STRING_STRING_STRING	ggn_marshal_VOID__STRING_STRING_STRING

G_END_DECLS

#endif /* __ggn_marshal_MARSHAL_H__ */

