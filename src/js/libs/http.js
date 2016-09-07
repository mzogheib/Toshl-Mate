var Q = require('../../node_modules/q/q');

module.exports = {
    get: get,
    post: post
};

// This could make good use of a factory to create all the functions

function get (url, auth) {
    var xhr = new XMLHttpRequest();
    var deferred = Q.defer();
    console.log('GET: ' + url);

    xhr.open('GET', url, true);
    if(auth) {
        xhr.setRequestHeader('Authorization', 'Basic ' + auth);
    }
    xhr.timeout = 20000;

    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 200) {
            // console.log(xhr.responseText);
            deferred.resolve(xhr.responseText);
        }
    };

    xhr.ontimeout = function () {
        console.log('url timeout');
    };

    xhr.send();

    return deferred.promise;
}

function post (url, payload, auth) {
    var xhr = new XMLHttpRequest();
    var deferred = Q.defer();
    console.log('POST: ' + url);
    // console.log(JSON.stringify(payload));

    xhr.open('POST', url, true);
    xhr.setRequestHeader('Authorization', 'Basic ' + auth);
    xhr.setRequestHeader('Content-type', 'application/json; charset=utf-8');
    xhr.setRequestHeader('Content-length', payload.length);
    xhr.setRequestHeader('Connection', 'close');
    xhr.timeout = 20000;

    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4 && xhr.status == 201) {
            deferred.resolve(xhr.responseText);
        }
    };

    xhr.ontimeout = function () {
        console.log('url timeout');
    };

    xhr.send(JSON.stringify(payload));

    return deferred.promise;
}