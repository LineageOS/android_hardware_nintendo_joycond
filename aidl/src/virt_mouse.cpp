#include "virt_mouse.h"

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <libevdev/libevdev-uinput.h>
#include <utils/Log.h>

#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/uinput.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using android::base::GetProperty;
using android::base::GetUintProperty;

virt_mouse::virt_mouse() {
    int ret;

    // Create a virtual evdev on which the uinput will be based
    virt_evdev = libevdev_new();
    if (!virt_evdev) {
        ALOGE("Failed to create virtual evdev");
        exit(1);
    }

    libevdev_set_name(virt_evdev, "Joycond Virtual Mouse");
    libevdev_enable_property(virt_evdev, INPUT_PROP_POINTER);

    libevdev_enable_event_type(virt_evdev, EV_KEY);
    libevdev_enable_event_code(virt_evdev, EV_KEY, BTN_MOUSE, NULL);
    libevdev_enable_event_code(virt_evdev, EV_KEY, BTN_LEFT, NULL);

    libevdev_enable_event_type(virt_evdev, EV_REL);
    libevdev_enable_event_code(virt_evdev, EV_REL, REL_X, NULL);
    libevdev_enable_event_code(virt_evdev, EV_REL, REL_Y, NULL);

    libevdev_set_id_vendor(virt_evdev, 0x057e);
    libevdev_set_id_product(virt_evdev, 0x2010);

    libevdev_set_id_bustype(virt_evdev, BUS_USB);
    libevdev_set_id_version(virt_evdev, 0x0000);

    ret = libevdev_uinput_create_from_device(
        virt_evdev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (ret) {
        ALOGE("Failed to create libevdev_uinput; %d", ret);
        exit(1);
    }

    ALOGI("Successfully registered virtual mouse vid: 0x057e pid: 0x2010");

    ready.store(true);

    if (pthread_create(&mouseThread, NULL, __mouseLoop, this)) {
        ALOGE("pthread_create failed!");
        return;
    }

    pthread_setname_np(mouseThread, "joycond_mouse_thread");
}

virt_mouse::~virt_mouse() {
    ready.store(false);
    pthread_join(mouseThread, NULL);

    libevdev_uinput_destroy(uidev);
    libevdev_free(virt_evdev);
}

void virt_mouse::sync_event(struct input_event ev) {
    libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);

    return;
}

void virt_mouse::relay_mouse_event(struct input_event ev) {
    switch (ev.code) {
    case ABS_RX:
        sense_x.store(ev.value *
                      std::stof(GetProperty(PROP_SENSE_X, DEFAULT_SENSE_X)));
        break;
    case ABS_RY:
        sense_y.store(ev.value *
                      std::stof(GetProperty(PROP_SENSE_Y, DEFAULT_SENSE_Y)));
        break;
    case BTN_TR2:
        libevdev_uinput_write_event(uidev, EV_KEY, BTN_MOUSE, ev.value);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        break;
    case BTN_TL2:
        libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, ev.value);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        break;
    default:
        /* Do nothing */
        break;
    }

    return;
}

void *virt_mouse::__mouseLoop(void *args) {
    float _sense_x;
    float _sense_y;

    virt_mouse *const self = static_cast<virt_mouse *>(args);

    while (self->ready.load()) {
        // reduce atomic blocks and ensure consistent values for process
        _sense_x = self->sense_x.load();
        _sense_y = self->sense_y.load();

        // write value if x or y is past dead zone else 0
        if (std::fabsf(_sense_x) >
                std::stof(GetProperty(PROP_DEAD_X, DEFAULT_DEAD_X)) ||
            std::fabsf(_sense_y) >
                std::stof(GetProperty(PROP_DEAD_Y, DEFAULT_DEAD_Y))) {
            libevdev_uinput_write_event(self->uidev, EV_REL, REL_X, _sense_x);
            libevdev_uinput_write_event(self->uidev, EV_REL, REL_Y, _sense_y);
        } else {
            libevdev_uinput_write_event(self->uidev, EV_REL, REL_X, 0);
            libevdev_uinput_write_event(self->uidev, EV_REL, REL_Y, 0);
        }

        libevdev_uinput_write_event(self->uidev, EV_SYN, SYN_REPORT, 0);

        usleep(GetUintProperty(PROP_POLL, uint32_t(DEFAULT_POLL)));
    }

    return NULL;
}
