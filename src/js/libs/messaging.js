var messageKeys = require('message_keys');

module.exports = {
    messageKeys: messageKeys,
    sendDict: sendDict
};  

// Send a dictionary of data to the Pebble
function sendDict(dictionary) {
    // Send
    Pebble.sendAppMessage(
        dictionary,
        function(e) {},
        function(e) {}
    );
}