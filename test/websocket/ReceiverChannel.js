/**
 * Created by manson on 15/2/25.
 */
var WebSocket = require('ws');
var ws = new WebSocket('ws://127.0.0.1:9439/channels/receiver1');

ws.on('open', function open() {
    console.log('receiver1 open!!!');
});

ws.on('message', function (data, flags) {
    // flags.binary will be set if a binary data is received.
    // flags.masked will be set if the data was masked.

    var obj = JSON.parse(data);
    if (obj.type == 'senderConnected') {
        console.log(obj.senderId, ' -> connected');
    } else if (obj.type == 'senderDisconnected') {
        console.log(obj.senderId, ' -> disconnected');
    } else if (obj.type == 'message') {
        obj.data = 'reply from receiver1';
        ws.send(JSON.stringify(obj));
    } else {
        console.log('unknow -> receiver1 received: ', data);
    }
});

ws.on('close', function close() {
    console.warn('receiver1 close!!!');
});