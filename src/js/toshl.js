var HTTP = require('./libs/http');
var Base64 = require('./libs/base64');
var Q = require('../node_modules/q/q');

module.exports = {
    create: create,
    get: list,
    list: list
};

var api = {
    baseUrl: 'https://api.toshl.com/'
};

function getAuth () {
    var config = JSON.parse(localStorage.getItem('clay-settings'));
    
    if(config) {
        return Base64.encode(config['AUTH'] + ':');
    } else {
        return null;
    }
}

function list (endpoint, parameters) {
    var deferred = Q.defer();
    var urlParams = [];

    for(var param in parameters) {
        urlParams.push(param + '=' + parameters[param]);
    }

    HTTP.get(api.baseUrl + endpoint + '?' + urlParams.join('&'), getAuth())
    .then(function (response) {
        console.log(response);
        deferred.resolve(JSON.parse(response));
    });

    return deferred.promise;
}

function create (endpoint, payload) {
    var deferred = Q.defer();
    console.log(JSON.stringify(payload));

    HTTP.post(api.baseUrl + endpoint, payload, getAuth())
    .then(function (response) {
        deferred.resolve(response);
    });

    return deferred.promise;
}
