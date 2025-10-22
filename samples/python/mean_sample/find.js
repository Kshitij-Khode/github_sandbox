var testSchema = require('../schemas/testSchema.js')

module.exports.findTestDocuments = function(req, res) {
	console.log('Finding the latest test documents based on req.query: '+req.query);
	testSchema.find(req.query).sort({date: -1}).exec(function(err, testDocs) {
		if(err) {
			console.log('[ERR]findInTests: '+err.message);
			return  res.status(500).end(); 
		}
		else if(!testDocs) {
			console.log('[ERR]findInTests(!testDocs): '+err.message);
			return res.status(404).end();
		}
		else {
			return res.end(JSON.stringify(testDocs));
		}
	});
}