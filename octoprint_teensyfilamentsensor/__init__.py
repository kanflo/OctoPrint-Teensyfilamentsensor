# coding=utf-8
from __future__ import absolute_import
import time
from threading import Thread
from datetime import datetime, timezone

### (Don't forget to remove me)
# This is a basic skeleton for your plugin's __init__.py. You probably want to adjust the class name of your plugin
# as well as the plugin mixins it's subclassing from. This is really just a basic skeleton to get you started,
# defining your plugin as a template plugin, settings and asset plugin. Feel free to add or remove mixins
# as necessary.
#
# Take a look at the documentation on what other plugin mixins are available.

from typing import *
from enum import Enum
import octoprint.plugin
from octoprint.events import Events
import usb.core
import usb.util


class FilamentSensorState(Enum):
    DISCONNECTED = 1  # Sensor USB device not found
    CONNECTED = 2     # Sensor USB device connected, no print ongoing
    SENSOR_ERROR = 3  # Error talking to sensor USB device
    FILAMENT_OK = 4   # Print active, filament ok
    FILAMENT_JAM = 5  # Print active, filament jammed (sensor detects no movement)


class TeensyfilamentsensorPlugin(
    octoprint.plugin.SettingsPlugin,
    octoprint.plugin.AssetPlugin,
    octoprint.plugin.TemplatePlugin,
    octoprint.plugin.StartupPlugin,
    octoprint.plugin.EventHandlerPlugin
):

    sensor_state: FilamentSensorState = FilamentSensorState.DISCONNECTED
    filament_counter: Optional[int] = None
    is_printing: bool = False


    def update_js_state(self, new_state: str):
        self._plugin_manager.send_plugin_message(self._identifier, dict(sensorState = new_state))


    def sensor_ui_thread(self):
        """A thread that updates the OctoPrint UI
        """
        self._logger.info("🔴🔴🔴 Teensy Filament Sensor thread is alive 🔴🔴🔴")
        last_state: FilamentSensorState = FilamentSensorState.FILAMENT_JAM
        while True:
            if last_state != self.sensor_state:
                # OK and JAM are only sent once, keeping track of last state makes us not send
                # the message to the JavaScript side of things when not needed-
                last_state = self.sensor_state
                if self.sensor_state == FilamentSensorState.FILAMENT_OK:
                    self.update_js_state("ok")
                elif self.sensor_state == FilamentSensorState.FILAMENT_JAM:
                    self.update_js_state("error")
            else:
                # The following states toggle the JS icon
                if self.sensor_state == FilamentSensorState.DISCONNECTED:
                    self.update_js_state("disconnected")
                    time.sleep(1)
                    self.update_js_state("idle")
                    time.sleep(1)
                elif self.sensor_state == FilamentSensorState.CONNECTED:
                    self.update_js_state("connected")
                    time.sleep(1)
                    self.update_js_state("idle")
                    time.sleep(1)
                elif self.sensor_state == FilamentSensorState.SENSOR_ERROR:
                    self.update_js_state("error")
                    time.sleep(1)
                    self.update_js_state("idle")
                    time.sleep(1)
                else:
                    time.sleep(0.25)

    def sensor_thread(self):
        """A thread that reads the Teensy sensor
        """
        dev = None
        while True:
            try:
                if dev is None:
                    dev = usb.core.find(idVendor=0x04c9, idProduct=0x1234)
                    if dev is not None:
                        if dev.is_kernel_driver_active(0):
                            dev.detach_kernel_driver(0)
                        dev.reset()
                        endpoint_in = dev[0][(0,0)][0]
                        if self.is_printing:
                            self.set_sensor_state(FilamentSensorState.FILAMENT_OK)
                        else:
                            self.set_sensor_state(FilamentSensorState.CONNECTED)
                else:
                    buffer = dev.read(endpoint_in.bEndpointAddress, 64, 1000).tobytes()
                    self.set_counter(buffer[6])
            except Exception as e:
                self._logger.error(f"Failed talking to Teensy : {e}")
                dev = None
            if dev is None:
                if self.is_printing:
                    self.set_sensor_state(FilamentSensorState.SENSOR_ERROR)
                else:
                    self.set_sensor_state(FilamentSensorState.DISCONNECTED)
                self.set_counter(None)
                time.sleep(1)
            else:
                time.sleep(0.5)


    def set_sensor_state(self, new_state: FilamentSensorState):
        if self.sensor_state != new_state:
#            if self.is_printing and self.sensor_state == FilamentSensorState.DISCONNECTED and new_state == FilamentSensorState.FILAMENT_OK:
#                # Override new state if the sensor is disconnected and we discover the print started
#                self._logger.info(f"🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠")
#                new_state = FilamentSensorState.SENSOR_ERROR
#            if self.is_printing and new_state == FilamentSensorState.DISCONNECTED:
#                # Override new state if the sensor is disconnected and we discover the print started
#                self._logger.info(f"🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠🟠")
#                new_state = FilamentSensorState.SENSOR_ERROR
            self._logger.info(f"🟠🟠🟠 New sensor state {self.sensor_state} -> {new_state} {self.is_printing} 🟠🟠🟠")
            self.sensor_state = new_state


    def set_counter(self, counter: Optional[int]):
        """Set the filament counter

        Args:
            counter (Optional[int]): Current value or None if not connected
        """
        if self.filament_counter != counter:
            self._logger.info(f"🔴🔴🔴 Counter is {counter} 🔴🔴🔴")
            self.filament_counter = counter


    def on_after_startup(self):
        self.filament_counter = None
        self._logger.info("🔴🔴🔴 Teensy Filament Sensor 🔴🔴🔴")
        self.thread = Thread(target = self.sensor_thread, args = ())
        self.ui_thread = Thread(target = self.sensor_ui_thread, args = ())
        self.thread.daemon = True
        self.thread.start()
        self.ui_thread.daemon = True
        self.ui_thread.start()
        # Later: self.thread.join()


    ##~~ SettingsPlugin mixin

    def get_settings_defaults(self):
        return {
            # put your plugin's default settings here
        }

    def printing_started(self, filament_timeout: int):
        self.filament_time = datetime.now(timezone.utc).timestamp()
        self.filament_timeout = filament_timeout
        self.is_printing = True
        self.start_counter = self.filament_counter if self.filament_counter is not None else 0
        self._logger.info(f"🟢🔴🟢🔴🟢🔴 Print started at {self.start_counter} ticks")
        self.set_sensor_state(FilamentSensorState.FILAMENT_OK)

    def printing_finished(self):
        self.is_printing = False
        self.set_sensor_state(FilamentSensorState.CONNECTED)
        self._logger.info(f"🟢🔴🟢🔴🟢🔴 Print stopped after {self.filament_counter - self.start_counter} ticks")


    ##~~ EventHandlerPlugin mixin
    def on_event(self, event: str, payload: dict):
        # self._logger.info(f"🔴🔴🔴 {event} {payload} 🔴🔴🔴")
        if event == Events.CLIENT_OPENED:
            self._plugin_manager.send_plugin_message(self._identifier, dict(sensorState = 1))
            return

        if not self.is_printing:
            # We take the first ZChange as an indicator that printing started
            # but allow for a 15 second timeout of filement movement as the bed shall move,
            # nozzle primed and what not.
            if event == "ZChange" and payload["old"] is None:
                self.printing_started(15)
                self.set_sensor_state(FilamentSensorState.FILAMENT_OK)
            elif event == "PrinterStateChanged" and payload["state_id"] == "RESUMING":
                self.printing_started(2)
        else:
            # Printing stops when when XY are homed or the user cancels or pauses the print
            if event == "Home" or (event == "PrinterStateChanged" and payload["state_id"] in ["CANCELLING", "PAUSING"]):
                self.printing_finished()

    ##~~ AssetPlugin mixin
    def get_assets(self):
        # Define your plugin's asset files to automatically include in the
        # core UI here.
        return {
            "js": ["js/teensyfilamentsensor.js"],
            "css": ["css/teensyfilamentsensor.css"],
            #"less": ["less/teensyfilamentsensor.less"]
        }

    ##~~ Softwareupdate hook

    def get_template_configs(self):
        return [
            dict(type="navbar", custom_bindings=False),
            dict(type="settings", custom_bindings=False)
        ]

    def get_update_information(self):
        # Define the configuration for your plugin to use with the Software Update
        # Plugin here. See https://docs.octoprint.org/en/master/bundledplugins/softwareupdate.html
        # for details.
        return {
            "teensyfilamentsensor": {
                "displayName": "Teensyfilamentsensor Plugin",
                "displayVersion": self._plugin_version,

                # version check: github repository
                "type": "github_release",
                "user": "kanflo",
                "repo": "OctoPrint-Teensyfilamentsensor",
                "current": self._plugin_version,

                # update method: pip
                "pip": "https://github.com/kanflo/OctoPrint-Teensyfilamentsensor/archive/{target_version}.zip",
            }
        }


# If you want your plugin to be registered within OctoPrint under a different name than what you defined in setup.py
# ("OctoPrint-PluginSkeleton"), you may define that here. Same goes for the other metadata derived from setup.py that
# can be overwritten via __plugin_xyz__ control properties. See the documentation for that.
__plugin_name__ = "Teensyfilamentsensor Plugin"


# Set the Python version your plugin is compatible with below. Recommended is Python 3 only for all new plugins.
# OctoPrint 1.4.0 - 1.7.x run under both Python 3 and the end-of-life Python 2.
# OctoPrint 1.8.0 onwards only supports Python 3.
__plugin_pythoncompat__ = ">=3,<4"  # Only Python 3

def __plugin_load__():
    global __plugin_implementation__
    __plugin_implementation__ = TeensyfilamentsensorPlugin()

    global __plugin_hooks__
    __plugin_hooks__ = {
        "octoprint.plugin.softwareupdate.check_config": __plugin_implementation__.get_update_information
    }
