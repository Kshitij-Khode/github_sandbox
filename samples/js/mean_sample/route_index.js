var insertDB = require('./insert.js');
var findDB = require('./find.js');


module.exports = function(app){
	app.use(function(req, res, next){
		next();
	});
	console.log('----- Inital Logging complete -----');
	app.get('/', function(req, res){res.render('main');});
	app.post('/insert/', insertDB.insertTestDocument);
	app.get('/findInSummaries/', findDB.findTestDocuments);
}	