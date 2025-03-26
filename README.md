This is a collection of ESPHome configurations that
I use in my own home. I have tried to modularise it
as far as seems reasonable, so it could serve as an
example of how this can be achieved.

In particular, I have tried to define configurations
for the various commercial products that should be
usable by anyone as is, with zero personal/local
configuration in the device YAML files. These can be
found under devices/.

Device-independent configuration, such as esphome
components included in all devices (wifi:, ota:, 
etc stanzas) or device-specific modbus registers
can be found in include/.

