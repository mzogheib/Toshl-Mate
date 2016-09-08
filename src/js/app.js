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
    var deferred = Q.defer();

    toshl.get('me')
        .then(function (response){
            // Get the financial date range
            var params = getDateRange(response['start_day']);

            return toshl.get('me/summary', params);
        })
        .then(function (response) {
            // Get the financial report for this range
            var expensesPlanned = response['expenses']['sum_planned'];
            var expensesTotal = response['expenses']['sum'];
            var expensesActual = expensesTotal - expensesPlanned;
            var from = new Date(response['from']).toString().substring(4, 10);
            var to = new Date(response['to']).toString().substring(4, 10);
            var expensesHeader = from + ' - ' + to;

            var report = {
                'EXPENSES_HEADER': expensesHeader,
                'EXPENSES_ACTUAL': expensesActual.toFixed(2),
                'EXPENSES_PLANNED': expensesPlanned.toFixed(2),
                'EXPENSES_TOTAL': expensesTotal.toFixed(2)
            };

            deferred.resolve(report);
        });

    return deferred.promise;

    function getDateRange(startDay) {
        // TODO: this is a bit of a nightmare but it works. Try to simplify it.
        var today = new Date();
        var endOfThisMonth = new Date(today.getFullYear(), today.getMonth() + 1, 0);

        var fromDate;
        var toDate;
        var endDay;

        if(startDay === 1) {
            endDay = endOfThisMonth.getDate();
            fromDate = new Date(today.getFullYear(), today.getMonth(), startDay);
            toDate = new Date(today.getFullYear(), today.getMonth(), endDay);
        } else {
            endDay = startDay - 1;

            var startOfThisMonth = new Date(today.getFullYear(), today.getMonth(), 1);

            var endOfLastMonth = new Date();
            endOfLastMonth.setDate(startOfThisMonth.getDate() - 1)

            var startOfNextMonth = new Date();
            startOfNextMonth.setDate(endOfThisMonth.getDate() + 1)

            var thisMonth = {
                startDate: new Date(today.getFullYear(), today.getMonth(), startDay),
                endDate: new Date(today.getFullYear(), today.getMonth(), endDay)
            };
            thisMonth.endDate = Math.min(thisMonth.endDate, endOfThisMonth)

            var lastMonth = {
                startDate: new Date(endOfLastMonth.getFullYear(), endOfLastMonth.getMonth(), startDay)
            };
            lastMonth.startDate = Math.min(lastMonth.startDate, startOfThisMonth)

            var nextMonth = {
                endDate: new Date(startOfNextMonth.getFullYear(), startOfNextMonth.getMonth(), endDay)
            };

            fromDate = new Date((today >= lastMonth.startDate && today <= thisMonth.endDate) ? lastMonth.startDate : thisMonth.startDate);
            toDate = new Date((today >= thisMonth.startDate && today <= nextMonth.endDate) ? nextMonth.endDate : thisMonth.endDate);
        }

        function formatDate(date) {
            // Source: http://stackoverflow.com/questions/10830357/javascript-toisostring-ignores-timezone-offset
            var tzoffset = date.getTimezoneOffset() * 60000;
            var localISOTime = (new Date(date - tzoffset)).toISOString().slice(0,-1);

            return localISOTime.substring(0, 10);
        }

        return {
            'from': formatDate(fromDate),
            'to': formatDate(toDate)
        }
    }
}
