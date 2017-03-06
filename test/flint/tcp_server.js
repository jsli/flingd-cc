var net = require('net');

var client = net.createConnection(9440, '127.0.0.1');

client.on('data', function (s) {
    console.log(s.toString())
});

client.on('connect', function (s) {
    client.write('from test client')
});

setTimeout(function() {
    client.end();
}, 5000);