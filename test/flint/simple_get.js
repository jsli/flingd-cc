/**
 * Created by manson on 15/3/2.
 */
var http = require('http');

http.get("http://127.0.0.1:9431/apps/~test", function (res) {
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