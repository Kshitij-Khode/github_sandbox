var testSchema = require('../schemas/testSchema.js');

module.exports.insertTestDocument = function(req, res) {
	var testDoc = new testSchema(req.body);
	testDoc.save(function(err, insertedDocument) {
		if(err) {
			console.log('[ERR]insertTestDocument: '+err.message);
			res.status(500).end();
		}
		else {
			console.log('[PASS]insertTestDocument(InsertSuccess):\n'+insertedDocument)
			res.status(200).end();
		}
	});
}