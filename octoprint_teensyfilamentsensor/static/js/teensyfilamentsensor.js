/*
 * View model for OctoPrint-Teensyfilamentsensor
 *
 * Author: Johan Kanflo
 * License: AGPLv3
 */
$(function() {
    function TeensyfilamentsensorViewModel(parameters) {
        var self = this;

        self.loginStateViewModel = parameters[0];
        self.settingsViewModel = parameters[1];
        self.oldState = "disconnected"


        self.filament_sensor_status = $("#filament_sensor_status");
        self.sensorState = ko.observable(undefined);

        self.onBeforeBinding = function() {
            self.settings = self.settingsViewModel.settings;
        };

        self.onDataUpdaterPluginMessage = function(plugin, data) {
            //console.error("Teensy JS!");
            //console.error(plugin);
            if (plugin != "teensyfilamentsensor") {
                return;
            }

            if (data.sensorState !== undefined) {
                self.sensorState(data.sensorState);
            }
        };

        self.onStartup = function () {
            //self.filament_sensor_status.removeClass("disconnected").addClass("on");
            //console.error("Teensy JS2!");
            self.sensorState.subscribe(function() {
                //console.error("***********************************")
                //console.error(self.sensorState())
                newState = self.sensorState()
                self.filament_sensor_status.removeClass(self.oldState).addClass(newState);
                self.oldState = newState
            });
        }
    }


    /* view model class, parameters for constructor, container to bind to
     * Please see http://docs.octoprint.org/en/master/plugins/viewmodels.html#registering-custom-viewmodels for more details
     * and a full list of the available options.
     */
    OCTOPRINT_VIEWMODELS.push({
        construct: TeensyfilamentsensorViewModel,
        // ViewModels your plugin depends on, e.g. loginStateViewModel, settingsViewModel, ...
        dependencies: ["settingsViewModel","loginStateViewModel"],
        elements: ["#navbar_plugin_teensyfilamentsensor","#settings_plugin_teensyfilamentsensor"]

        //dependencies: [ /* "loginStateViewModel", "settingsViewModel" */ ],
        // Elements to bind to, e.g. #settings_plugin_teensyfilamentsensor, #tab_plugin_teensyfilamentsensor, ...
        //elements: [ /* ... */ ]
    });
});
