# CTProdikeysMac
A driver that enables the Creative Prodikeys to function on the macOS 10.14 platform

Currently a WIP

Issues
1. Since the Kexts you need to build are not signed by Apple, You must have SIP disbaled (which will prevent most of you from using this--and that's okay!)
2. The HID Devices are not properly released, and the MIDI list is refreshed everytime the MIDI server is activated

