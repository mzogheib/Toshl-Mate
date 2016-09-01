module.exports = function () {
    var clayConfig = this; 

    clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
        // If AUTH changes the either delete all favourites or confirm it's the same account
    });
};