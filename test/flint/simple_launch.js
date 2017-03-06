// launch a web page and stop it in 5s

var http = require('http');
var querystring = require('querystring');

var appInfo = {
    url: 'http://www.baidu.com',
    useIpc: false,
    maxInactive: 5 * 1000
};

var data = {
    type: 'launch',
    app_info: appInfo
};

var postData = JSON.stringify(data);

var options = {
    hostname: '127.0.0.1',
    port: 9431,
    path: '/apps/~test',
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
    });
});

req.on('error', function (e) {
    console.log('problem with request: ' + e.message);
});


console.log(postData);
// write data to request body
req.write(postData);
req.end();