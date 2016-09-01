var Clay = require('pebble-clay');
var clayConfig = require('./config/config');
var clayConfigCustom = require('./config/custom-clay');
var clay = new Clay(clayConfig, clayConfigCustom, { autoHandleEvents: false });

var messaging = require('./libs/messaging');

Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
    if (e && !e.response) {
        return;
    }

    // Get the keys and values from each config item
    var dict = clay.getSettings(e.response);
    var dictString = decodeURIComponent(e.response);
    var dictJSON = JSON.parse(dictString);

    console.log('Passed dict: ' + dictString);
    // Send
    messaging.sendDict(dict);
});

Pebble.addEventListener("ready", function(e) {
    console.log("Hello world! - Sent from your javascript application.");
});
