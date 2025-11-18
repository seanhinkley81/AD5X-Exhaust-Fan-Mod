**AD5X Exhaust Fan Mod with Moonraker HTTP Control for Zmod**

The AD5X has the same frame as the Adventurer Pro and can support a fan. However, the motherboard is missing the headers. Not wanting to solder a connector onto my new printer, I came up with a less invasive alternative.

I'm using a Moonraker HTTP power API with an ESP-01 and a relay hat to turn the fan on and off. The main limitation is the inability to control the fan speed; it is 0% or 100%.

**Required Components**

To start, you will need:

 *An ESP8266-01 (or ESP-01) with a relay hat

 *Wiring
 
 *A 24V to 5V buck converter (I had one lying around)
 
 *A 40mm 24V fan (also had lying around)
 
 *M3x14 screws (4)
 

**Setup and Configuration**
Flash the ESP-01:
Flash the necessary firmware to the ESP using the Arduino IDE.
You will need the ESP8266 board libraries.
Select "ESP8266 Generic" as the board type.
use the moonraker.ino file to flash via arudiono ide

You will need to change the Wi-Fi credentials in the code to your specific Wi-Fi information.

**Moonraker Configuration (AD5X/Zmod):**

Add the provided user.moonraker.conf code to the configuration file located at mod_data/user.moonraker.conf on your AD5X running Zmod.

Add the provided user.cfg code to the configuration file located at mod_data/user.cfg.

**Wiring**

The 24V fan should be on one side of the relay switch with the 24V power.
The fan's ground should connect to the system ground.

I used spade connectors on the build plate power supply (PSU) to connect my buck converter to the 24V line.

I then wired the relay up to the converter: 24V on the fan side of the relay, and 5V from the buck converter to power the ESP-01.

**Installation & Placement**
Fan Installation
Secure the fan using the M3x16 Screws.

Electronics Placement
Buck Converter: I wrapped the buck converter in clear tape to prevent short circuits.

ESP Relay Modules: The ESP relay modules were placed inside a case found on Thingiverse
https://www.thingiverse.com/thing:7104017


**Sealing the Enclosure**
To ensure proper ventilation and prevent exhaust air from recirculating into the chamber, you must seal the large open space where the PTFE tubes enter the print chamber.
Create the Seal: Use craft foam to create a cover.
Attachment: Glue the AD5X enclosure ring (the part around the existing tube hole) to the foam.
Trimming: Trim the foam to match the enclosure ring's size.
Slits for Tubes: Make a single horizontal cut and then 1-20 vertical cuts into the foam to allow the PTFE tubes to pass through while still creating a seal against the opening.

