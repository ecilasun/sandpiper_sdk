

#include <linux/kref.h>
#include <linux/list.h>

struct sock;

#define TLS_TOE_DEVICE_NAME_MAX		32


struct tls_toe_device {
	char name[TLS_TOE_DEVICE_NAME_MAX];
	struct list_head dev_list;
	int  (*feature)(struct tls_toe_device *device);
	int  (*hash)(struct tls_toe_device *device, struct sock *sk);
	void (*unhash)(struct tls_toe_device *device, struct sock *sk);
	void (*release)(struct kref *kref);
	struct kref kref;
};

int tls_toe_bypass(struct sock *sk);
int tls_toe_hash(struct sock *sk);
void tls_toe_unhash(struct sock *sk);

void tls_toe_register_device(struct tls_toe_device *device);
void tls_toe_unregister_device(struct tls_toe_device *device);
