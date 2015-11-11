var mongoose = require('mongoose');
var Schema   = mongoose.Schema;

var testSchema = new Schema({
    start_time: Date,
    result: String,
    test_log: {},
    test_host: String,
    test_steps: {},
    test_name: String,
    test_id: Number,
    logtar: String,
    test_values: {},
    test_description: String,
    end_time: Date,
    section_id: Number,
    section_name: String,
    tunnel_type: String,
    run_id: String,
    tags: [],
    platform: [String],
    platform_version: [String],
    build: [String]
});

module.exports = mongoose.model('testSchema', testSchema);