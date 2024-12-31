#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>

#include <android-base/properties.h>
#include <utils/Log.h>

#include "ctlr_detector.h"
#include "ctlr_mgr.h"
#include "epoll_mgr.h"

#include "Joycond.h"

namespace aidl::android::hardware::nintendo::joycond {

using ::android::base::GetBoolProperty;
using ::android::base::GetProperty;
using ::android::base::SetProperty;
using ::ndk::ScopedAStatus;

Joycond::Joycond() {
    parseLayoutFromFile();
    mMapping.combined = GetBoolProperty(PROP_COMBINED, true);
    mMapping.analog = GetBoolProperty(PROP_ANALOG, true);
    mMapping.rsmouse = GetBoolProperty(PROP_RSMOUSE, true);

    // ensure props set for first run
    SetProperty(PROP_COMBINED, mMapping.combined ? "1" : "0");
    SetProperty(PROP_ANALOG, mMapping.analog ? "1" : "0");
    SetProperty(PROP_RSMOUSE, mMapping.rsmouse ? "1" : "0");

    ready.store(true);
    if (pthread_mutex_init(&mapLock, NULL)) {
        ALOGE("pthread_mutex_init failed!");
        return;
    }

    if (pthread_create(&pollThread, NULL, __threadLoop, this)) {
        ALOGE("pthread_create failed!");
        pthread_mutex_destroy(&mapLock);
        return;
    }

    pthread_setname_np(pollThread, "joycond_poll_thread");
}

Joycond::~Joycond() {
    ready.store(false);
    pthread_join(pollThread, NULL);
    pthread_mutex_destroy(&mapLock);
}

::ndk::ScopedAStatus Joycond::restartService() {
    int ret;

    ready.store(false);
    ret = pthread_join(pollThread, NULL);
    if (ret) {
        return ScopedAStatus::fromServiceSpecificError(ret);
    }

    ready.store(true);
    ret = pthread_create(&pollThread, NULL, __threadLoop, this);
    if (ret) {
        return ScopedAStatus::fromServiceSpecificError(ret);
    }

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::setLayout(const std::vector<KeyMap> &layout) {
    std::string str = "";

    pthread_mutex_lock(&mapLock);
    for (auto &entry : layout) {
        ALOGI("Mapping %d to %d", entry.from, entry.to);
        mMapping.layout[entry.from] = entry.to;
        str +=
            std::to_string(entry.from) + "," + std::to_string(entry.to) + ";";
    }
    pthread_mutex_unlock(&mapLock);
    str.back() = '\0'; // get rid of trailing ;

    std::ofstream writer(FILE_LAYOUT);
    writer << str << std::endl;
    writer.close();

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::getLayout(std::vector<KeyMap> *_aidl_return) {
    pthread_mutex_lock(&mapLock);
    for (auto &entry : mMapping.layout) {
        KeyMap k;
        k.from = entry.first;
        k.to = entry.second;
        _aidl_return->push_back(k);
    }
    pthread_mutex_unlock(&mapLock);
    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::setCombined(bool combined) {
    pthread_mutex_lock(&mapLock);
    mMapping.combined = combined;
    pthread_mutex_unlock(&mapLock);
    SetProperty(PROP_COMBINED, combined ? "1" : "0");

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::getCombined(bool *_aidl_return) {
    pthread_mutex_lock(&mapLock);
    *_aidl_return = mMapping.combined;
    pthread_mutex_unlock(&mapLock);

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::setAnalog(bool analog) {
    pthread_mutex_lock(&mapLock);
    mMapping.analog = analog;
    pthread_mutex_unlock(&mapLock);
    SetProperty(PROP_ANALOG, analog ? "1" : "0");

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::getAnalog(bool *_aidl_return) {
    pthread_mutex_lock(&mapLock);
    *_aidl_return = mMapping.analog;
    pthread_mutex_unlock(&mapLock);

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::setRsmouse(bool rsmouse) {
    pthread_mutex_lock(&mapLock);
    mMapping.rsmouse = rsmouse;
    pthread_mutex_unlock(&mapLock);
    SetProperty(PROP_RSMOUSE, rsmouse ? "1" : "0");

    return ScopedAStatus::ok();
}

::ndk::ScopedAStatus Joycond::getRsmouse(bool *_aidl_return) {
    pthread_mutex_lock(&mapLock);
    *_aidl_return = mMapping.rsmouse;
    pthread_mutex_unlock(&mapLock);

    return ScopedAStatus::ok();
}

void *Joycond::__threadLoop(void *args) {
    Joycond *const self = static_cast<Joycond *>(args);

    epoll_mgr epoll_manager;
    ctlr_mgr ctlr_manager(epoll_manager, &(self->mMapping), &(self->mapLock));
    ctlr_detector ctlr_detector(ctlr_manager, epoll_manager);

    while (self->ready.load()) {
        epoll_manager.loop();
    }

    return NULL;
}

void Joycond::parseLayoutFromFile() {
    int ret;
    char *token;
    std::string buf;
    std::string readLayout;

    // check if folder exists
    struct stat st = {0};
    if (stat(FOLDER_LAYOUT, &st) == -1) {
        mkdir(FOLDER_LAYOUT, 0644);
    }

    if (stat(FILE_LAYOUT, &st) == -1) {
        std::ofstream writer(FILE_LAYOUT);
        writer << DEFAULT_LAYOUT << std::endl;
        writer.close();
        readLayout = DEFAULT_LAYOUT;
    } else {
        std::ifstream reader(FILE_LAYOUT);
        while (std::getline(reader, buf)) {
            readLayout += buf;
        }

        reader.close();
    }

    std::stringstream stream(readLayout);
    std::string tok;
    while (!stream.eof()) {
        const char *ctok = tok.c_str();
        getline(stream, tok, ';');
        ALOGI("Got pairing %s", ctok);

        std::pair<uint32_t, uint32_t> mPair;
        ret = sscanf(ctok, "%d,%d", &mPair.first, &mPair.second);
        if (ret != 2)
            ALOGE("Failed to parse pair from %s", ctok);
        else
            mMapping.layout.insert(mPair);
    }
}

} // namespace aidl::android::hardware::nintendo::joycond
