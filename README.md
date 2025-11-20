This is a collection of ESPHome configurations that
I use in my own home. I have tried to modularise it
as far as seems reasonable, so it could serve as an
example of how this can be achieved.

In particular, I have tried to define configurations
for the various commercial products that should be
usable by anyone as is, with zero personal/local
configuration in the device YAML files. These can be
found under devices/.

Personalised configurations are available in the
top-level directory, for visibility in the ESPHome
Device Builder web ui.

Device-independent configuration, such as esphome
components included in all devices (wifi:, ota:, 
etc stanzas) or peripheral device modbus registers
that don't depend on the actual ESPHome device
that is querying them can be found in include/.

For what it is worth, I tend not to use the Device
Builder, but rather ssh to either the Home Assistant
container to edit files and maintain the git repo,
or to the esphome container to actually run compilation
or upload for the devices.

I have enabled both the HA ssh container, as well as the
supervisor SSH access on port 22222 (https://developers.home-assistant.io/docs/operating-system/debugging/)
The latter is required to be able to access docker containers.

This is what my ~/.ssh/config file looks like to make
this as simple as possible:

```
Host hass
        User root
        Hostname homeassistant.local
        ForwardAgent yes

Host esphome
        User root
        Port 22222
        Hostname homeassistant.local
        RequestTTY yes
        RemoteCommand /usr/bin/docker exec -it $(docker ps | grep esphome | cut -f1 -d" ") /bin/bash
```
I'm no docker expert, so there is probably a simpler
command to get the container ID. It does work, at least.

I ssh to "hass" to edit files (in /config/esphome) and
to "esphome" to run the esphome cli.

When doing bulk updates (e.g. when there is a new version
of ESPHome), I do something like:

```
for i in *.yaml; do
  esphome compile $i && (esphome upload $i &)
done
```

This has the effect of serialising compilation of
each configuration (to make as many cores available
to the compiler as possible), while uploading the
compiled firmware to the device in the background.

