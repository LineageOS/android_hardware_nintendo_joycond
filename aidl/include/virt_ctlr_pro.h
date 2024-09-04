#ifndef JOYCOND_VIRT_CTLR_PRO
#define JOYCOND_VIRT_CTLR_PRO

#include "Joycond.h"
#include "epoll_mgr.h"
#include "phys_ctlr.h"
#include "virt_ctlr.h"
#include "virt_mouse.h"

#include "cutils/properties.h"

#include <libevdev/libevdev.h>
#include <map>
#include <memory>

class virt_ctlr_pro : public virt_ctlr {
  private:
    std::shared_ptr<phys_ctlr> phys;
    epoll_mgr &epoll_manager;
    std::shared_ptr<epoll_subscriber> subscriber;
    struct libevdev *virt_evdev;
    struct libevdev_uinput *uidev;
    int uifd;
    std::map<int, struct ff_effect> rumble_effects;
    std::string mac;

    struct mapping *mMapping;
    pthread_mutex_t *mapLock;

    virt_mouse *mouse;

    void relay_events(std::shared_ptr<phys_ctlr> phys);
    void handle_uinput_event();

  public:
    virt_ctlr_pro(std::shared_ptr<phys_ctlr> phys, epoll_mgr &epoll_manager,
                  struct mapping *mMapping, pthread_mutex_t *mapLock);
    virtual ~virt_ctlr_pro();

    virtual void handle_events(int fd);
    virtual bool
    contains_phys_ctlr(std::shared_ptr<phys_ctlr> const ctlr) const;
    virtual bool contains_phys_ctlr(char const *devpath) const;
    virtual bool contains_fd(int fd) const;
    virtual std::vector<std::shared_ptr<phys_ctlr>> get_phys_ctlrs();
    virtual int get_uinput_fd();
    virtual void remove_phys_ctlr(const std::shared_ptr<phys_ctlr> phys);
    virtual void add_phys_ctlr(std::shared_ptr<phys_ctlr> phys);
    virtual enum phys_ctlr::Model needs_model();
    virtual bool supports_hotplug() { return true; }
    virtual bool set_player_led(int index, bool on);
    virtual bool set_all_player_leds(bool on);
    virtual bool set_player_leds_to_player(int player);
};

#endif
