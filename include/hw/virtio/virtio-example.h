/*
 * Virtio EXAMPLE Support
 *
 * Copyright Red Hat, Inc. 2019
 * Copyright Yoni Bettan <ybettan@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * (at your option) any later version.  See the COPYING file in the
 * top-level directory.
 */


/*
 * ============================================================================
 *                             Device specification
 * ============================================================================
 *
 * 5.2 Example Device
 *
 * The virtio example device is a simple virtual block device. Read and write
 * requests are placed in the queue, and serviced (probably out of order) by
 * the device except where noted.
 *
 * 5.2.1 Device ID
 *
 * 21
 *
 * 5.2.2 Virtqueues
 *
 * 0 requestq
 *
 * 5.2.3 Feature bits
 *
 * None currently defined.
 *
 * 5.2.4 Device configuration layout
 *
 * None currently defined.
 *
 * 5.2.5 Device Initialization
 *
 * 1. The virtqueue is initialized.
 *
 * 5.2.6 Device Operation
 *
 * When the driver need computation, it places a request that consist of both
 * input and output buffer into the queue.
 * The first buffer is used as an input for the device and contain a single
 * integer, represented by 4 or 8 bytes (depends on the architecture), and the
 * second is given for the device to fill the result in it.
 *
 * The device is increasing this integer by 1, meaning the LSB will be
 * increased by 1 and if needed the carry will be carried to the next byte.
 *
 * If the device get a number that is out of the range of an integer only the
 * lower bytes that fit the size of an integer will represent the input and the
 * upper bytes will be ignored.
 * If the result is out of range then only the lower bytes will be written as
 * result as well.
 *
 * 5.2.6.1 Driver Requirements: Device Operation
 *
 * The driver MUST place the 2 buffers in the same request in order to make a
 * request atomically handled by the device, the first buffer contains the
 * input and should be read-only and the second should be empty and write-only.
 *
 * 5.2.6.2 Device Requirements: Device Operation
 *
 * The device MUST place the result inside the output buffer allocated by the
 * driver.
 */

#ifndef QEMU_VIRTIO_EXAMPLE_H
#define QEMU_VIRTIO_EXAMPLE_H

#define VIRTIO_ID_EXAMPLE 21

#define TYPE_VIRTIO_EXAMPLE "virtio-example-device"
#define VIRTIO_EXAMPLE(obj) \
        OBJECT_CHECK(VirtIOEXAMPLE, (obj), TYPE_VIRTIO_EXAMPLE)
#define VIRTIO_EXAMPLE_GET_PARENT_CLASS(obj) \
        OBJECT_GET_PARENT_CLASS(obj, TYPE_VIRTIO_EXAMPLE)

typedef struct VirtIOEXAMPLE {
    VirtIODevice parent_obj;

    /* Only one vq - guest puts buffer(s) on it when it needs computation */
    VirtQueue *vq;

} VirtIOEXAMPLE;

#endif
