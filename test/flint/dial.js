/**
 * Created by manson on 15/3/6.
 */
// launch a web page and stop it in 5s

var http = require('http');

var luanch = function () {
    var options = {
        hostname: '127.0.0.1',
        port: 9431,
        path: '/apps/youtube',
        method: 'POST',
        headers: {
            'Content-Length': 9
        }
    };

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

// write data to request body
    req.write('test_data');
    req.end();
};

var stop = function () {
    var options = {
        hostname: '127.0.0.1',
        port: 9431,
        path: '/apps/youtube',
        method: 'DELETE'
    };

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

    req.end();
};

luanch();

setTimeout(function () {
    stop();
}, 10 * 1000);