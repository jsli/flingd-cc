// launch a web page and stop it in 5s

var http = require('http');
var querystring = require('querystring');

var stop = function (token) {
    var appInfo = {
        url: 'app:?/Users/manson/temp/a.out',
        useIpc: false,
        maxInactive: 0
    };

    var data = {
        type: 'launch',
        app_info: appInfo
    };

    var postData = JSON.stringify(data);

    var options = {
        hostname: '127.0.0.1',
        port: 9431,
        path: '/apps/~test/run',
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': postData.length,
            'Authorization': token
        }
    };

    console.log("send stop ::::::::: ", token);
    var req = http.request(options, function (res) {
        console.log('STATUS: ' + res.statusCode);
        console.log('HEADERS: ' + JSON.stringify(res.headers));
        res.setEncoding('utf8');
        res.on('data', function (chunk) {
            console.log('BODY: ' + chunk);
        });
    });

    req.on('error', function (e) {
        console.log('problem with request: ' + e.message);
    });


    console.log(postData);
// write data to request body
    req.write(postData);
    req.end();
};

var join = function() {
    var appInfo = {
        url: 'app:?/Users/manson/temp/a.out',
        useIpc: false,
        maxInactive: 0
    };

    var data = {
        type: 'join',
        app_info: appInfo
    };

    var postData = JSON.stringify(data);

    var options = {
        hostname: '127.0.0.1',
        port: 9431,
        path: '/apps/~test/run',
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': postData.length
        }
    };

    var req = http.request(options, function (res) {
        console.log('STATUS: ' + res.statusCode);
        console.log('HEADERS: ' + JSON.stringify(res.headers));
        res.setEncoding('utf8');
        res.on('data', function (chunk) {
            console.log('BODY: ' + chunk);
            data = JSON.parse(chunk);
            setTimeout(function() {
                stop(data.token);
            }, 5000);
        });
    });

    req.on('error', function (e) {
        console.log('problem with request: ' + e.message);
    });


    console.log(postData);
// write data to request body
    req.write(postData);
    req.end();
};

join();