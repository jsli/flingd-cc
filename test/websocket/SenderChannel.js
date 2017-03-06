/**
 * Created by manson on 15/2/25.
 */
var WebSocket = require('ws');

var ws1 = new WebSocket('ws://127.0.0.1:9439/channels/receiver1/senders/sender1');
ws1.on('open', function open() {
    console.log('sender1 open!!!');
    ws1.send('from sender1');
});
ws1.on('message', function (data, flags) {
    console.log('sender1 received: ', data);
});
ws1.on('close', function close() {
    console.warn('sender1 close!!!');
});

//var ws2 = new WebSocket('ws://127.0.0.1:9439/channels/receiver1/senders/sender2');
//ws2.on('open', function open() {
//    console.log('sender2 open!!!');
//    ws2.send('from sender2');
//});
//ws2.on('message', function (data, flags) {
//    console.log('sender2 received: ', data);
//});
//ws2.on('close', function close() {
//    console.warn('sender2 close!!!');
//});