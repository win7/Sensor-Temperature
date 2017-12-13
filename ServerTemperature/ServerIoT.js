/* framework NODEJS */
var server_io = require('http').createServer(handler),
    io = require('socket.io').listen(server_io),
    fs = require('fs');

var net = require("net");
var server_net = net.createServer();

var PORT_IO = 3000;
var PORT_NET = 3001;

var connect_time_out = 4; /* minute */
var socket_net;

/* Server IO */
server_io.listen(PORT_IO, function () {
    console.log("Server IO listening at port %d", PORT_IO);
});

io.sockets.on('connection', function(socket) {
    var address = socket.handshake.address;
    console.log("Connect IO %s %d",  address.address, address.port);
    try {
        socket_net.write("R\n");
    }
    catch(err) {
        console.log("Arduino is disconnect");
    }
    socket.on('update range', function(max, min) {
        console.log("update range");
        socket_net.write("r," + max + "," + min + "\n");
    });
    socket.on('disconnect', function() {
        console.log("Disconnect IO");
    });
});

function handler(req, res) {
    fs.readFile(__dirname+'/index.html', function(err, data) {
        if (err) {
            console.log(err);
            res.writeHead(500);
            return res.end('Error loading index.html');
        }
        res.writeHead(200);
        res.end(data);
    });
}

/* Server Net */
server_net.listen(PORT_NET, function() {
    console.log("Server NET listening at port %d", PORT_NET);
    server_net.maxConnections = 10;
    server_net.on("connection", HandleSocket);
});

function HandleSocket(socket) {
    socket_net = socket;
    console.log("Connect NET %s %d", socket.remoteAddress, socket.remotePort);
    socket.setEncoding("utf8");
    socket.write("auth\n");
    socket.on("data", onSocketData);
    socket.once("close", onSocketClose);
    socket.on("error", onSocketError);
    socket.setTimeout(connect_time_out * 60 * 1000, function() {  
        console.log("Time out %s", socket.ID);
        socket.destroy();
    });
    function onSocketData(data_) {
        var data = data_.split(",");
        console.log("Data NET: %s", data);
        if(data[0] == "t"){
            var date = new Date().getTime();
            var temp = parseFloat(data[1]);
            io.sockets.emit('temperature update', date, temp);
        }else{
            if(data[0] == "r"){
                io.sockets.emit('temperature range', parseInt(data[1]), parseInt(data[2]));
            }
        }
    }

    function onSocketClose() {
        console.log("Disconnect NET");
    }

    function onSocketError(err) {
        console.log("Socket NET error %s", err.message);
    }
}