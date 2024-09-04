#ifndef JOYCOND_VIRT_MOUSE
#define JOYCOND_VIRT_MOUSE

#define PROP_SENSE_X "persist.vendor.joycond.mouse_sense.x"
#define PROP_SENSE_Y "persist.vendor.joycond.mouse_sense.y"
#define DEFAULT_SENSE_X "0.0005"
#define DEFAULT_SENSE_Y "0.0005"

#define PROP_DEAD_X "persist.vendor.joycond.mouse_dead.x"
#define PROP_DEAD_Y "persist.vendor.joycond.mouse_dead.y"
#define DEFAULT_DEAD_X "5"
#define DEFAULT_DEAD_Y "5"

#include <atomic>
#include <memory>

#include <libevdev/libevdev.h>
#include <pthread.h>

#include "cutils/properties.h"

#include "epoll_mgr.h"
#include "phys_ctlr.h"
#include "virt_ctlr.h"

class virt_mouse {
  private:
    static void *__mouseLoop(void *args);

    std::atomic<bool> ready;
    std::atomic<float> sense_x;
    std::atomic<float> sense_y;
    pthread_t mouseThread;

    struct libevdev *virt_evdev;
    struct libevdev_uinput *uidev;

  public:
    virt_mouse();
    ~virt_mouse();

    void sync_event(struct input_event ev);

    // Takes RS event and processes into an event for our virtual mouse
    void relay_mouse_event(struct input_event ev);
};

#endif
