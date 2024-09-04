/*
 * Copyright (C) 2024 Thomas Makin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JOYCOND_H
#define JOYCOND_H

#define PROP_COMBINED "persist.vendor.joycond.combined"
#define PROP_ANALOG "persist.vendor.joycond.analog"
#define PROP_RSMOUSE "persist.vendor.joycond.rsmouse"

#define FOLDER_LAYOUT "/data/vendor/joycond/"
#define FILE_LAYOUT \
    FOLDER_LAYOUT \
    "layout.txt"
#define DEFAULT_LAYOUT                                                         \
    "304,304;305,305;307,307;308,308;309,309;310,310;311,311;312,312;313,313;" \
    "314,314;315,315;316,316;317,317;318,318"

#include <atomic>
#include <map>
#include <pthread.h>

#include <aidl/android/hardware/nintendo/joycond/BnJoycond.h>

using aidl::android::hardware::nintendo::joycond::KeyMap;

struct mapping {
    std::map<uint32_t, uint32_t> layout;
    bool combined;
    bool analog;
    bool rsmouse;
};

namespace aidl::android::hardware::nintendo::joycond {

struct Joycond : public BnJoycond {
    Joycond();
    ~Joycond();

    ::ndk::ScopedAStatus restartService() override;

    ::ndk::ScopedAStatus setLayout(const std::vector<KeyMap> &layout) override;

    ::ndk::ScopedAStatus getLayout(std::vector<KeyMap> *_aidl_return) override;

    ::ndk::ScopedAStatus setCombined(bool combined) override;

    ::ndk::ScopedAStatus getCombined(bool *_aidl_return) override;

    ::ndk::ScopedAStatus setAnalog(bool analog) override;

    ::ndk::ScopedAStatus getAnalog(bool *_aidl_return) override;

    ::ndk::ScopedAStatus setRsmouse(bool rsmouse) override;

    ::ndk::ScopedAStatus getRsmouse(bool *_aidl_return) override;

    struct mapping mMapping;
    pthread_mutex_t mapLock;

  private:
    static void *__threadLoop(void *args);
    void parseLayoutFromFile();

    std::atomic<bool> ready;
    pthread_t pollThread;
};
} // namespace aidl::android::hardware::nintendo::joycond

#endif
