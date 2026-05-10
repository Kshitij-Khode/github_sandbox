var express = require('express');
var app = express();
var path = require('path');
var bodyParser = require('body-parser');
var mongoose   = require('mongoose');
var mongoServerUrl = require('./conf.js').mongoConnectionString();

app.use(express.static(path.join(__dirname, 'views', 'public')));
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());

var connect = function () {
	var options = { server: { socketOptions: { keepAlive: 1 } } };
	mongoose.connect(mongoServerUrl, options);
};

connect();

require('./routes/route_index')(app);

app.get('/read/:collection/:trid', function(req, res){
	// console.log('Reading from ' + req.params.collection + ' with trid: ' + req.params.trid);
	res.status(200).end();
});

app.listen(3000);
