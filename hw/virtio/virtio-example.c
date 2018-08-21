/*
 * A virtio device example.
 *
 * Copyright 2019 Red Hat, Inc.
 * Copyright 2019 Yoni Bettan <ybettan@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * (at your option) any later version.  See the COPYING file in the
 * top-level directory.
 */

#include "qemu/osdep.h"
#include "qemu/iov.h"
#include "hw/virtio/virtio.h"
#include "hw/virtio/virtio-example.h"


/*
 * this function is called when the driver 'kick' the virtqueue.
 * since we can have more than 1 virtqueue we need the vq argument in order to
 * know which one was kicked by the driver.
 */
static void handle_input(VirtIODevice *vdev, VirtQueue *vq)
{
    VirtQueueElement *elem;
    int data;

    /*
     * get the virtqueue element sent from the driver.
     * in_sg are the driver inputs (device outputs)
     * out_sg are the driver output (device input)
     */
    elem = virtqueue_pop(vq, sizeof(VirtQueueElement));

    /* read the driver output sg (device input sg) into a buffer */
    iov_to_buf(elem->out_sg, elem->out_num, 0, &data, sizeof(int));

    /* process the data */
    data++;

    /* write the result to the driver input sg (device output sg) */
    iov_from_buf(elem->in_sg, elem->in_num, 0, &data, sizeof(int));

    /* push back the result into the virtqueue */
    virtqueue_push(vq, elem, 1);

    /* interrupt the driver */
    virtio_notify(vdev, vq);

    return;
}

/*
 * This function gets the host features as a parameter and add to it all the
 * features supported by the device.
 * This example-device has no currently defined feature bits but we still need
 * this function because when a device is plugged this function is called to
 * check the features offer by the device so it must exist and return the
 * host features without any change.
 */
static uint64_t
get_features(VirtIODevice *vdev, uint64_t features, Error **errp)
{
    return features;
}

static void virtio_example_device_realize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOEXAMPLE *vexmp = VIRTIO_EXAMPLE(dev);

    /* common virtio device initialization */
    virtio_init(vdev, "virtio-example", VIRTIO_ID_EXAMPLE, 0);

    /* this device suppot 1 virtqueue */
    vexmp->vq = virtio_add_queue(vdev, 1, handle_input);
}

static void virtio_example_device_unrealize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);

    /* common virtio device cleanup */
    virtio_cleanup(vdev);
}

static void virtio_example_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    vdc->realize = virtio_example_device_realize;
    vdc->unrealize = virtio_example_device_unrealize;
    vdc->get_features = get_features;
}

static const TypeInfo virtio_example_info = {
    .name = TYPE_VIRTIO_EXAMPLE,
    .parent = TYPE_VIRTIO_DEVICE,
    .instance_size = sizeof(VirtIOEXAMPLE),
    .class_init = virtio_example_class_init,
};

static void virtio_register_types(void)
{
    type_register_static(&virtio_example_info);
}

type_init(virtio_register_types)
