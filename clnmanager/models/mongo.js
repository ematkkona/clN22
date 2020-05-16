var mongoose = require("mongoose");

mongoose.connect('mongodb://localhost:27017/clnmanagerdb', { useUnifiedTopology: true, useNewUrlParser: true, useCreateIndex: true })

var mongoSchema =   mongoose.Schema;

var workitemSchema  = {
	"workitem": String,
	"result": String,
	"offer": { type: String, default: '0' },
	"wuid": String,
	"startp": { type: String, default: Date.now() },
	"startw": String,
	"finish": String,
	"duratw": String,
	"duratp": String
};

module.exports = mongoose.model('workitem',workitemSchema);;
