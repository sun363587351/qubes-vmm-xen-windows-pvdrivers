/******************************************************************************
 * xen_gntmem.h
 *
 * A library for granting pages on the local machine to foreign domains
 *
 * Copyright (c) 2009 C Smowton <chris.smowton@cl.cam.ac.uk>
 */

#ifndef XEN_GNTMEM_H
#define XEN_GNTMEM_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT32 grant_ref_t;
typedef UINT32 grant_handle_t;
typedef UINT16 domid_t;
typedef unsigned int evtchn_port_t;
struct gntmem_handle;

/* Open gntmem driver; returns a handle for use in all other calls, or INVALID_HANDLE_VALUE on error */
struct gntmem_handle* gntmem_open();

/* Close the device, and rescind all grants issued against this device.
   If you wish to be able to independently rescind multiple seperately issued grants,
   you must open the device more than once. */

void gntmem_close(struct gntmem_handle* h);

/* All following functions return 0 on success or -1 otherwise. */

/* Set device-instance h to permit at most new_limit pages to be granted */
int gntmem_set_local_quota(struct gntmem_handle* h, int new_limit);

/* Set the gntmem device as a whole to permit a total of new_limit pages to be granted */
int gntmem_set_global_quota(struct gntmem_handle* h, int new_limit);

/* Grant a number of pages to a nominated domain. Both allocates and shares those
   pages. There is no support for sharing an existing page, because the allocation
   is performed in the kernel driver.
   grants_out should be a pointer to an array of grant_ref_t large enough to handle n_pages grants. 
   address_out will be assigned the user-accessible address of the granted pages on success. 
   Be aware that the pages so granted are virtually but not necessarily (pseudo-)physically contiguous.
   The return value is an opaque pointer to a handle representing this grant, or NULL on error,
   in which case the out parameters are assigned nothing useful.

   If the thread in which this is called terminates, the shared region will be rescinded as if
   gntmem_rescind_grants had been called, and access to (*grants_out) will result in a page fault.
   */
void* gntmem_grant_pages_to_domain(struct gntmem_handle* h, domid_t domain, int n_pages, grant_ref_t* grants_out);

void* gntmem_grant_pages_to_domain_notify(struct gntmem_handle* h, domid_t
        domain, int n_pages, int notify_offset, evtchn_port_t notify_port,
        grant_ref_t* grants_out);


int gntmem_map_foreign_pages(struct gntmem_handle* h, struct ioctl_gntmem_map_foreign_pages *in, struct ioctl_gntmem_map_foreign_pages_out *out);
int gntmem_unmap_foreign_pages(struct gntmem_handle* h, struct ioctl_gntmem_unmap_foreign_pages *in);

#ifdef __cplusplus
}
#endif

#endif /* XEN_GNTMEM_H */
