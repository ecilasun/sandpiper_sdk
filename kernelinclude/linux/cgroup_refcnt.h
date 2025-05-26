
CGROUP_REF_FN_ATTRS
void css_get(struct cgroup_subsys_state *css)
{
	if (!(css->flags & CSS_NO_REF))
		percpu_ref_get(&css->refcnt);
}
CGROUP_REF_EXPORT(css_get)


CGROUP_REF_FN_ATTRS
void css_get_many(struct cgroup_subsys_state *css, unsigned int n)
{
	if (!(css->flags & CSS_NO_REF))
		percpu_ref_get_many(&css->refcnt, n);
}
CGROUP_REF_EXPORT(css_get_many)


CGROUP_REF_FN_ATTRS
bool css_tryget(struct cgroup_subsys_state *css)
{
	if (!(css->flags & CSS_NO_REF))
		return percpu_ref_tryget(&css->refcnt);
	return true;
}
CGROUP_REF_EXPORT(css_tryget)


CGROUP_REF_FN_ATTRS
bool css_tryget_online(struct cgroup_subsys_state *css)
{
	if (!(css->flags & CSS_NO_REF))
		return percpu_ref_tryget_live(&css->refcnt);
	return true;
}
CGROUP_REF_EXPORT(css_tryget_online)


CGROUP_REF_FN_ATTRS
void css_put(struct cgroup_subsys_state *css)
{
	if (!(css->flags & CSS_NO_REF))
		percpu_ref_put(&css->refcnt);
}
CGROUP_REF_EXPORT(css_put)


CGROUP_REF_FN_ATTRS
void css_put_many(struct cgroup_subsys_state *css, unsigned int n)
{
	if (!(css->flags & CSS_NO_REF))
		percpu_ref_put_many(&css->refcnt, n);
}
CGROUP_REF_EXPORT(css_put_many)
