#include "phys_ctlr.h"

#include <android-base/logging.h>
#include <fcntl.h>
#include <glob.h>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utils/Log.h>

// private
std::optional<std::string>
phys_ctlr::get_first_glob_path(std::string const &pattern) {
    glob_t globbuf;

    glob(pattern.c_str(), 0, NULL, &globbuf);
    if (globbuf.gl_pathc) {
        std::string match(globbuf.gl_pathv[0]);
        globfree(&globbuf);
        return {match};
    }

    ALOGI("no match found for %s", pattern.c_str());
    globfree(&globbuf);
    return std::nullopt;
}

std::optional<std::string> phys_ctlr::get_led_path(std::string const &name) {
    return get_first_glob_path(std::string("/sys/") + devpath +
                               "/device/leds/*" + name);
}

void phys_ctlr::init_leds() {
    std::optional<std::string> tmp;
    for (unsigned int i = 0; i < 100; i++) {
        tmp = get_led_path("player1");
        if (tmp.has_value()) {
            player_leds[0].open(tmp.value() + "/brightness");
            if (!player_leds[0].is_open()) {
                ALOGE("Failed to open player1 led brightness");
                usleep(10);
                continue;
            }
            player_led_triggers[0].open(tmp.value() + "/trigger");
            if (!player_led_triggers[0].is_open()) {
                ALOGE("Failed to open player1 trigger");
                usleep(10);
                continue;
            }
            break;
        }
        usleep(10);
    }

    for (unsigned int i = 0; i < 100; i++) {
        tmp = get_led_path("player2");
        if (tmp.has_value()) {
            player_leds[1].open(tmp.value() + "/brightness");
            if (!player_leds[1].is_open()) {
                ALOGE("Failed to open player2 led brightness");
                usleep(10);
                continue;
            }
            player_led_triggers[1].open(tmp.value() + "/trigger");
            if (!player_led_triggers[1].is_open()) {
                ALOGE("Failed to open player2 trigger");
                usleep(10);
                continue;
            }
            break;
        }
        usleep(10);
    }

    for (unsigned int i = 0; i < 100; i++) {
        tmp = get_led_path("player3");
        if (tmp.has_value()) {
            player_leds[2].open(tmp.value() + "/brightness");
            if (!player_leds[2].is_open()) {
                ALOGE("Failed to open player3 led brightness");
                usleep(10);
                continue;
            }
            player_led_triggers[2].open(tmp.value() + "/trigger");
            if (!player_led_triggers[2].is_open()) {
                ALOGE("Failed to open player3 trigger");
                usleep(10);
                continue;
            }
            break;
        }
        usleep(10);
    }

    for (unsigned int i = 0; i < 100; i++) {
        tmp = get_led_path("player4");
        if (tmp.has_value()) {
            player_leds[3].open(tmp.value() + "/brightness");
            if (!player_leds[3].is_open()) {
                ALOGE("Failed to open player4 led brightness");
                usleep(10);
                continue;
            }
            player_led_triggers[3].open(tmp.value() + "/trigger");
            if (!player_led_triggers[3].is_open()) {
                ALOGE("Failed to open player4 trigger");
                usleep(10);
                continue;
            }
            break;
        }
        usleep(10);
    }

    if (model != Model::Left_Joycon) {
        for (unsigned int i = 0; i < 100; i++) {
            tmp = get_led_path("home");
            if (tmp.has_value()) {
                home_led.open(tmp.value() + "/brightness");
                if (!home_led.is_open()) {
                    ALOGE("Failed to open home led brightness");
                    usleep(10);
                    continue;
                }
                break;
            }
            usleep(10);
        }
    }
}

void phys_ctlr::handle_event(struct input_event const &ev) {
    int type = ev.type;
    int code = ev.code;
    int val = ev.value;

    if (type != EV_KEY)
        return;

    switch (model) {
    case Model::Procon:
    case Model::Snescon:
        switch (code) {
        case BTN_TL:
            l = val;
            break;
        case BTN_TL2:
            zl = val;
            break;
        case BTN_TR:
            r = val;
            break;
        case BTN_TR2:
            zr = val;
            break;
        case BTN_START:
            plus = val;
            break;
        case BTN_SELECT:
            minus = val;
            break;
        default:
            break;
        }
        break;
    case Model::Sio:
        switch (code) {
        case BTN_TL:
            l = val;
            break;
        case BTN_TL2:
            zl = val;
            break;
        case BTN_TR:
            r = val;
            break;
        case BTN_TR2:
            zr = val;
            break;
        default:
            break;
        }
        break;
    case Model::Left_Joycon:
        switch (code) {
        case BTN_TL:
            l = val;
            break;
        case BTN_TL2:
            zl = val;
            break;
        case BTN_TR:
            sl = val;
            break;
        case BTN_TR2:
            sr = val;
            break;
        default:
            break;
        }
        break;
    case Model::Right_Joycon:
        switch (code) {
        case BTN_TL:
            sl = val;
            break;
        case BTN_TL2:
            sr = val;
            break;
        case BTN_TR:
            r = val;
            break;
        case BTN_TR2:
            zr = val;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

// public
phys_ctlr::phys_ctlr(std::string const &devpath, std::string const &devname)
    : devpath(devpath), devname(devname), evdev(nullptr), is_serial(false) {

    zero_triggers();

    int fd = open(devname.c_str(), O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        ALOGE("Failed to open %s; errno=%d", devname.c_str(), errno);
        exit(1);
    }
    if (libevdev_new_from_fd(fd, &evdev)) {
        ALOGE("Failed to create evdev from fd");
        exit(1);
    }

    int product_id = libevdev_get_id_product(evdev);
    // Extra checks are required for charging grip
    if (product_id == 0x200e) {
        ALOGI("Found Charging Grip Joy-Con...");
        if (libevdev_has_event_code(evdev, EV_KEY, BTN_TL))
            product_id = 0x2006;
        else
            product_id = 0x2007;
    }

    switch (product_id) {
    case 0x2009:
        model = Model::Procon;
        ALOGI("Found Pro Controller");
        break;
    case 0x2006:
        model = Model::Left_Joycon;
        ALOGI("Found Left Joy-Con");
        break;
    case 0x2007:
        model = Model::Right_Joycon;
        ALOGI("Found Right Joy-Con");
        break;
    case 0x2017:
        model = Model::Snescon;
        ALOGI("Found SNES Controller");
        break;
    case 0xf123:
        model = Model::Sio;
        ALOGI("Found Switch Lite");
        break;
    default:
        model = Model::Unknown;
        ALOGE("Unknown product id = 0x%04x", libevdev_get_id_product(evdev));
        break;
    }

    if (model != Model::Sio)
        init_leds();

    // Prevent other users from having access to the evdev until it's paired
    grab();
    if (fchmod(get_fd(), S_IRUSR | S_IWUSR))
        ALOGE("Failed to change evdev permissions; %s", strerror(errno));

    // Check if this is a serial joy-con
    std::ifstream fname("/sys/" + devpath + "/name");
    std::string driver_name;
    std::getline(fname, driver_name);
    ALOGI("driver_name: %s", driver_name.c_str());
    if (driver_name.find("Serial") != std::string::npos) {
        ALOGI("Serial joy-con detected");
        // Turn off player LEDs by default with serial joycons by default
        set_all_player_leds(false);
        is_serial = true;
    } else if (model == Model::Sio) {
        ALOGI("Setting Sio as serial, ignoring lights...");
        is_serial = true;
    }

    // Attempt to read MAC address from uniq attribute
    mac_addr = "";
    std::ifstream funiq("/sys/" + devpath + "/uniq");
    std::getline(funiq, mac_addr);
    ALOGI("MAC: %s", mac_addr.c_str());
}

phys_ctlr::~phys_ctlr() {
    if (evdev) {
        int fd = libevdev_get_fd(evdev);
        libevdev_free(evdev);
        close(fd);
    }
}

bool phys_ctlr::set_player_led(int index, bool on) {
    if (index > 3 || !player_leds[index].is_open() || is_serial)
        return false;

    player_leds[index] << (on ? '1' : '0');
    player_leds[index].flush();
    return true;
}

bool phys_ctlr::set_all_player_leds(bool on) {
    for (int i = 0; i < 4; i++) {
        if (!set_player_led(i, on))
            return false;
        usleep(5000);
    }
    return true;
}

bool phys_ctlr::set_player_leds_to_player(int player) {
    if (player < 1 || player > 4) {
        ALOGE("%d is not a valid player led value", player);
        return false;
    }

    set_all_player_leds(false);
    for (int i = 0; i < player; i++) {
        set_player_led(i, true);
        usleep(5000);
    }
    return true;
}

bool phys_ctlr::set_home_led(unsigned short brightness) {
    if (brightness > 15 || !home_led.is_open())
        return false;

    home_led << brightness;
    home_led.flush();
    return true;
}

bool phys_ctlr::blink_player_leds() {
    /* start with all player leds off */
    set_all_player_leds(false);

    for (int i = 0; i < 4; i++) {
        try {
            player_led_triggers[i] << "timer";
            player_led_triggers[i].flush();
        } catch (std::exception &e) {
            ALOGE("Failed to select LED timer trigger. Is ledtrig-timer module "
                  "probed?");
            return false;
        }
    }
    return true;
}

int phys_ctlr::get_fd() { return libevdev_get_fd(evdev); }

void phys_ctlr::handle_events() {
    struct input_event ev;

    int ret = libevdev_next_event(evdev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
    while (ret == LIBEVDEV_READ_STATUS_SYNC ||
           ret == LIBEVDEV_READ_STATUS_SUCCESS) {
        if (ret == LIBEVDEV_READ_STATUS_SYNC) {
            ALOGI("handle sync");
            while (ret == LIBEVDEV_READ_STATUS_SYNC) {
                handle_event(ev);
                ret = libevdev_next_event(evdev, LIBEVDEV_READ_FLAG_SYNC, &ev);
            }
        } else if (ret == LIBEVDEV_READ_STATUS_SUCCESS) {
            handle_event(ev);
        }
        ret = libevdev_next_event(evdev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
    }
}

enum phys_ctlr::PairingState phys_ctlr::get_pairing_state() const {
    enum phys_ctlr::PairingState state = PairingState::Pairing;

    bool combined = ::property_get_int32("persist.joycond.combined", 1);

    if (libevdev_get_id_product(evdev) == 0x200e)
        return PairingState::Waiting;

    if (model == Model::Sio)
        return PairingState::Virt_Procon;

    // uart joy-cons should just always be willing to pair
    if (is_serial)
        return PairingState::Waiting;

    switch (model) {
    case Model::Procon:
    case Model::Snescon:
        state = PairingState::Virt_Procon;
        break;
    case Model::Left_Joycon:
        state = PairingState::Waiting;
        if (!combined)
            state = PairingState::Horizontal;
        break;
    case Model::Right_Joycon:
        state = PairingState::Waiting;
        if (!combined)
            state = PairingState::Horizontal;
        break;
    default:
        break;
    }
    return state;
}

void phys_ctlr::zero_triggers() {
    l = zl = r = zr = sl = sr = plus = minus = 0;
}
