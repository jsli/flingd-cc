var net = require('net');

var client = net.createConnection(9440, '127.0.0.1');

client.on('data', function (s) {
    console.log(s.toString())
});

client.on('connect', function (s) {
    // single message
//    client.write('5:abcde');

    // uncomplete message
//    client.write('5:abcd');
//    setTimeout(function () {
//        client.write('e');
//    }, 2000);

    // mutil-message
//    client.write('5:abcde10:abcdefghij');


    // uncomplete mutil-message
//    client.write('5:abcd');
//    setTimeout(function () {
//        client.write('e5:12345');
//    }, 2000);

//    client.write('5:abcde5:qwert10:');
//    setTimeout(function () {
//        client.write('1234567890');
//    }, 2000);

    // illegal message
//    client.write('xx:abcde5:qwert');
//    setTimeout(function () {
//        client.write('10:1234567890');
//    }, 2000);

    // sticky message
    client.write('5:abcde');
    client.write('5:qwert');
});

//setTimeout(function() {
//    client.end();
//}, 5000);