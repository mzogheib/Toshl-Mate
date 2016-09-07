var Clay = require('pebble-clay');
var clayConfig = require('./config/config');
var clayConfigCustom = require('./config/custom-clay');
var clay = new Clay(clayConfig, clayConfigCustom, { autoHandleEvents: false });
var Q = require('../../node_modules/q/q');

var messaging = require('./libs/messaging');
var toshl = require('./toshl');

Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
    if (e && !e.response) {
        return;
    }

    // Get the keys and values from each config item
    clay.getSettings(e.response);

    // Check if the app is authorised
    var config = JSON.parse(localStorage.getItem('clay-settings'));

    if(config && config['AUTH']) {
        getFinancialReport()
            .then(function (dict) {
                messaging.sendDict(dict);
            });
    } else {
        messaging.sendDict({
            'NO_AUTH': 0
        });
    }
});

Pebble.addEventListener("ready", function(e) {
    // Check if the app is authorised
    var config = JSON.parse(localStorage.getItem('clay-settings'));

    if(config && config['AUTH']) {
        getFinancialReport()
            .then(function (dict) {
                messaging.sendDict(dict);
            });
    } else {
        messaging.sendDict({
            'NO_AUTH': 0
        });
    }
});

function getFinancialReport() {
    // 1. Get the financial date range.
    // 2. Get the sums for this range
    // 3. Send to the watch

    var deferred = Q.defer();

    toshl.get('me')
        .then(function (response) {
            // TODO: hard coded for now. Calculate this dynamically
            // var start_day = response['start_day'];

            var from_date = '2016-09-01';
            var to_date = '2016-09-30';

            var params = {
                from: from_date,
                to: to_date
            };

            return toshl.get('me/summary', params);
        })
        .then(function (response) {
            var expenses_planned = response['expenses']['sum_planned'];
            var expenses_total = response['expenses']['sum'];
            var expenses_actual = expenses_total - expenses_planned;
            var from = new Date(response['from']).toString().substring(4, 10);
            var to = new Date(response['to']).toString().substring(4, 10);
            var expenses_header = from + ' - ' + to;

            var report = {
                'EXPENSES_HEADER': expenses_header,
                'EXPENSES_ACTUAL': expenses_actual.toFixed(2),
                'EXPENSES_PLANNED': expenses_planned.toFixed(2),
                'EXPENSES_TOTAL': expenses_total.toFixed(2)
            };

            deferred.resolve(report);
        });

    return deferred.promise;
}
