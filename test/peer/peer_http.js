/**
 * Created by manson on 15/3/2.
 */
var http = require('http');

var get = function (path) {
    http.get('http://127.0.0.1:9433' + path, function (res) {
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

var post = function (method, path, data) {
    data = '';
    var options = {
        hostname: '127.0.0.1',
        port: 9433,
        path: path,
        method: method,
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


// write data to request body
    req.write(data);
    req.end();
};

var wsconnect = function() {

};

//get('/peerjs/id');
//get('/peerjs/peers');

post('post', '/peerjs/abcdefg/1q2w3e4r5t6y/id');

/*
post('post', '/peerjs/abcdefg/1q2w3e4r5t6y/id');
post('post', '/peerjs/abcdefg/1q2w3e4r5t6y/offer');
post('post', '/peerjs/abcdefg/1q2w3e4r5t6y/candidate');
post('post', '/peerjs/abcdefg/1q2w3e4r5t6y/answer');
post('post', '/peerjs/abcdefg/1q2w3e4r5t6y/leave');
*/

// 1. handle get [$key/id]

//get('http://127.0.0.1:9433/key/id');
//get("http://127.0.0.1:9433/peerjs/id");
//get("http://127.0.0.1:9433/peerjs/id?ts=14284640414000.9002381208042339");

// 2. handle post [$key/$id/$token/id]
//get('http://127.0.0.1:9433/key/id/token/id');

// 3. handle get [$key/peers]
//get('http://127.0.0.1:9433/key/peers');

// 4. handle post [$key/$id/$token/offer]
//get('http://127.0.0.1:9433/key/id/token/offer');

// 5. handle post [$key/$id/$token/candidate]
//get('http://127.0.0.1:9433/key/id/token/candidate');

// 6. handle post [$key/$id/$token/answer]
//get('http://127.0.0.1:9433/key/id/token/answer');

// 7. handle post [$key/$id/$token/leave]
//get('http://127.0.0.1:9433/key/id/token/leave');


// invalidate
//get('http://127.0.0.1:9433/key/invalidate');
//get('http://127.0.0.1:9433/key/id/token/invalidate');
