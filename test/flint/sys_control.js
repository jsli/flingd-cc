// launch a web page and stop it in 5s

var http = require('http');

var volume1 = {
    type: 'SET_VOLUME',
    level: 0.2
};

var volume2 = {
    type: 'SET_VOLUME',
    level: 0.8
};

var muted1 = {
    type: 'SET_MUTED',
    muted: true
};

var muted2 = {
    type: 'SET_MUTED',
    muted: false
};

var get_volume = {
    type: 'GET_VOLUME'
};

var get_mute = {
    type: 'GET_MUTED'
};


var v1Data = JSON.stringify(volume1);
var v2Data = JSON.stringify(volume2);

var getDataV = JSON.stringify(get_volume);
var getDataM = JSON.stringify(get_mute);

var m1Data = JSON.stringify(muted1);
var m2Data = JSON.stringify(muted2);

var request = function (data) {
    var options = {
        hostname: '192.168.1.103',
        port: 9431,
        path: '/system/control/',
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': data.length
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

    console.log(data);
    req.write(data);
    req.end();
};

var getVolume = function () {
    http.get("http://192.168.1.103:9431/system/control/", function (res) {
        console.log("Got response:\n" + res.statusCode);
        if (res.statusCode == 200) {
            var size = 0;
            var chunks = [];
            res.on('data', function (chunk) {
                size += chunk.length;
                chunks.push(chunk);
            });
            res.on('end', function () {
                var data = Buffer.concat(chunks, size);
                console.log(data.toString())
            });
        }
    }).on('error', function (e) {
        console.log("Got error: " + e.message);
    });
};

//request(v1Data);
//setTimeout(function() {
//    request(getDataV);
//}, 3000);

request(m2Data);
setTimeout(function() {
    request(getDataM);
}, 3000);
