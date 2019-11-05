var Service, Characteristic;

module.exports = function(homebridge) {
    Service = homebridge.hap.Service;
    Characteristic = homebridge.hap.Characteristic;
    homebridge.registerAccessory("homebridge-nodemcu-roomba876", "Roomba876", Roomba876Accessory);
}

/**
 * Roomba876Accessory
 *
 * @method Roomba876Accessory
 * @param {Object} log
 * @param {Object} config config.json
 * @param {String} config.name Name of Roomba to show in Home app
 * @param {String} config.blid Roomba's BLID
 * @param {String} config.password Roomba's password
 * @param {String} config.hostname Roomba's IP address
 * @param {String} config.model Model of Roomba to show in Home app
 *
 */
function Roomba876Accessory(log, config) {

    this.log        = log;
    this.name       = config["name"];
    this.hostname   = config["hostname"];
    this.model      = config["model"];

    log("Initialised Roomba with Name: [%s] Hostname: [%s] Model: [%s]", this.name, this.hostname, this.model);
}

/**
 * Roomba876Accessory
 *
 * Provides the following functions:
 * setPowerState()
 * getPowerState()
 * getIsCharging()
 * getBatteryLevel()
 * identify()
 * getServices()
 */
Roomba876Accessory.prototype = {

    /**
     * setPowerState
     *
     * @method setPowerState
     * @param {int} state 0 or 1
     * @param {function} callback
     *
     * Setting state to 0 will pause the Roomba and request that it docks.
     *
     */
    setPowerState: function(state, callback) {

        var log = this.log;

        log("Request to set power state to [%s]", state);

        if (state) {
            log("Starting roomba...");

            fetch(`${this.hostname}/start`)
                .then(response => response.json())
                .then(data => {
                    log("Roomba started");
                    log(data);
                    callback();
                })
                .catch(err => {
                    log("Failed to start roomba");
                    log(err);
                    callback(err);
                });
        } else {
            log("Docking roomba...");

            fetch(`${this.hostname}/dock`)
                .then(response => response.json())
                .then(data => {
                    log("Roomba is docking...");
                    log(data);
                    callback();
                })
                .catch(err => {
                    log("Failed to dock roomba");
                    log(err);
                    callback(err);
                });
        }
    },

    /**
     * getPowerState
     *
     * @method getPowerState
     * @param {function} callback
     * @return {Number} status
     *
     * Returns 1 if cleanMissionStatus.phase = run
     *
     */
    getPowerState: function(callback) {

        var log = this.log;

        log("Power state requested for Roomba");

        fetch(`${this.hostname}/status`)
            .then(response => response.json())
            .then(data => {
                log(`Roomba status is ${data.status}`);
                log(data);
                callback(null, data.status);
            })
            .catch(err => {
                log("Failed to get roomba status");
                log(err);
                callback(err);
            });
    },

    /**
     * getPowerState
     *
     * @method getPowerState
     * @param {function} callback
     * @return {Characteristic.ChargingState}
     *
     */
    getIsCharging: function(callback) {

        var log = this.log;

        log("Charging status requested for Roomba");

        fetch(`${this.hostname}/status`)
            .then(response => response.json())
            .then(data => {
                log(`Roomba charging status is ${data.is_charging}`);
                log(data);
                callback(null, (data.is_charging) ? Characteristic.ChargingState.CHARGING : Characteristic.ChargingState.NOT_CHARGING);
            })
            .catch(err => {
                log("Failed to get roomba charging status");
                log(err);
                callback(err);
            });
    },

    /**
     * getBatteryLevel
     *
     * @method getBatteryLevel
     * @param {function} callback
     * @return {Number} batteryLevel
     *
     * Returns a float representation of the battery level
     *
     */
    getBatteryLevel: function(callback) {

        var log = this.log;

        log("Battery level requested for Roomba");

        fetch(`${this.hostname}/status`)
            .then(response => response.json())
            .then(data => {
                log(`Roomba battery level is ${data.battery_level}`);
                log(data);
                callback(null, data.battery_level);
            })
            .catch(err => {
                log("Failed to get roomba battery level");
                log(err);
                callback(err);
            });
    },

    /**
     * identify
     *
     * Nort implemented. This method will always return successful to keep Siri happy.
     *
     * @method identify
     * @param {function} callback
     *
     */
    identify: function(callback) {
        this.log("Identify requested. Not supported yet.");
        callback();
    },

    /**
     * getServices
     *
     * Roomba876 supports the following Services:
     * AccessoryInformation
     * BatteryService
     * Switch
     *
     * @method identify
     * @param {function} callback
     * @return {Array} services
     *
     */
    getServices: function() {

        var log = this.log;

        log("Services requested");

        /* Roomba Information */
        // Populates the info box for the accessory in the Home app
        var accessoryInfo = new Service.AccessoryInformation();
        accessoryInfo.setCharacteristic(Characteristic.Manufacturer, "iRobot");
        accessoryInfo.setCharacteristic(Characteristic.SerialNumber, "MY-AWESOME-ROOMBA-876");
        accessoryInfo.setCharacteristic(Characteristic.Identify, false); // Disable idenitify capability
        accessoryInfo.setCharacteristic(Characteristic.Name, this.name); // Pull from conifguration
        accessoryInfo.setCharacteristic(Characteristic.Model, this.model); // Pull from configuration

        /* Battery Service */
        // Allows for Siri commands such as "Is the Roomba charging?" or "What is the battery percentage of the Roomba?"
        var batteryService = new Service.BatteryService();
        batteryService.getCharacteristic(Characteristic.BatteryLevel).on('get', this.getBatteryLevel.bind(this));
        batteryService.getCharacteristic(Characteristic.ChargingState).on('get', this.getIsCharging.bind(this));

        /* Switch Service */
        // Supports our on/off for the Roomba
        var switchService = new Service.Switch(this.name);
        switchService.getCharacteristic(Characteristic.On).on('get', this.getPowerState.bind(this)).on('set', this.setPowerState.bind(this));

        log("Reporting that we support AccessoryInformation, SwitchService and BatteryService");
        return [accessoryInfo, switchService, batteryService];
    }
}
