# VariWizard

VariWizard is an open-source repository designed to simplify the control of VariKey devices via the HID interface. 

Our platform offers developers a user-friendly way to harness these customizable HID devices and showcases a comprehensive API for their interaction.

Join VariWizard and become the wizard of the HID world!

## Usage

Get help:
```bash
sudo ./wizard --help
```

List available devices:
```bash
sudo ./wizard -l /dev/hidraw
```
Possible response:
```bash
list devices
unique                   hardware                 firmware                 pid     vid     path
0x00000000(           0) 0.0/0                    0.0.0                    0x0024  0xbeef  /dev/hidraw4
```

To scan or control devices with a 'pid' other than 0x0024 or a 'vid' other than 0xbeef, please set the filter parameter. 
For instance, use the '-p' option for 'pid' and the '-v' option for 'vid' with your respective decimal values.
Wildcards like input is not possible.

Please create a `config.json` file with simple content in the binary directory.

```json
{
    "device": "/dev/hidraw",
    "serial": "100500"
}
```

You need to set proper values for the HID device path on your Linux system and the device's unique serial number from the list above.

Get device Temperature:
```bash
sudo ./wizard -t
```
Possible response:
```bash
list devices
device 4168870248 temperature 19.648
```
