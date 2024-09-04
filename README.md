# android_hardware_nintendo_joycond

`joycond` is a daemon which uses the evdev devices provided by hid-nintendo (formerly known as hid-joycon) to implement joycon pairing.
This is the Android-specific AIDL-based fork for LineageOS.

`hid-nintendo` is currently upstream in mainline Linux, but this is intended for the updated downstream k4.9 version.

# Usage

This version is intended to be controlled by Android userspace control panels and props.
The latest version is built to allow in-place updating of layouts and settings.

# License

Upstream `joycond` and all core logic is licensed under the [GNU GPL v3 license](LICENSE.gpl)
AOSP AIDL boilerplate code and various build infrastructure is licensed under the [Apache v2 License](LICENSE.apache)
