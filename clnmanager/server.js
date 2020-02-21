// SPDX-License-Identifier: GPL-3.0-only
// clN22 workitem manager v0.91
// (c)2019-2020 EM~ eetu@kkona.xyz

require('dotenv').config()
const https = require('https');
const express = require('express');
const bodyParser = require('body-parser');
const mongoOp = require('./models/mongo');
const router = express.Router();
const fs = require('fs');
const port = process.env.PORT || 8022;
const version = process.env.VERSION;
const app = express();
var mongoose = require('mongoose');
var moment = require('moment');
const locks = require("mongo-locks");
locks.init(mongoose.connection);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({"extended" : false}));

if(!version){
        console.log(`ERROR! Check your .env -file`);
        process.exit(1);
}

if(process.env.wLstAdmins) {
	global.wLstAdmins = process.env.wLstAdmins.split(',');
} else {
	console.log(`Warning: No IPs specified to allow DB maintenance: Blocked from everyone.`);
	global.wLstAdmins = "*";
}
if(process.env.wLstNodes) {
	global.wLstNodes = process.env.wLstNodes.split(',');
} else {
	console.log(`Warning: No IPs specified to allow manage work items: It's free for all.`);
	global.wLstNodes = "*";
}
if(process.env.wLstWorkers) {
	global.wLstWorkers = process.env.wLstWorkers.split(',');
} else {
	console.log(`Warning: No IPs specified to allow updating work items: It's free for all.`);
	global.wLstWorkers = "*";
}

if(port == 8022) {
        console.log(`Note: Using default port.`);
}

function resRIP(remote) {
	var ipprefix = "::ffff:";
	if(remote){
		return(remote.replace(/^.*:/, ''));
	} else {
		return('*');
	}
}

function cleanExp(remote) {
	if(!remote) {
		remote="scheduled";
	}
	var consresp='';
	mongoOp.deleteMany({ result: 'expired'}, function (err,data){
		if(err) {
			consresp = "["+remote+":clean] Error deleting expired data";
		} else {
			consresp = "["+remote+":clean] Expired data deleted";
		}
		console.log(consresp);
	});
}
setInterval(cleanExp,60000);

function dropDb() {
	mongoOp.db.dropDatabase();
	var response = {"drop" : "All gone"};
	return(response);
}
router.get("/",function(req,res){
	var rootMsg = {"clN22-manager" : version};
	res.json(rootMsg);
});

router.route("/workitem")
        .get(function(req,res){
                var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
                var response = {};
                if(wLstAdmins.indexOf(requestIP) >= 0 || wLstNodes.indexOf(requestIP) >= 0 || wLstWorkers.indexOf(requestIP) >= 0) {
                        mongoOp.find({},function(err,data){
                                if(err) {
                                        response = {"error" : "Error fetching data"};
                                        res.status(503).send(response);
                                } else {
                                        response = {"message" : data};
                                        res.json(response);
                                }
                        });
                } else {
                        response = {"error" : "Access denied"};
                        res.status(403).send(response);
                        console.log(`[_workitem/All] Unauthorized request`);
                }
        })

	.post(function(req,res){
		var response = {};
		var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
		if(wLstNodes == "*" || wLstNodes.indexOf(requestIP) >= 0) {
			var db = new mongoOp();
			db.startp = moment();
			db.workitem = req.body.workitem;
			db.result = req.body.result;
			db.offer = '0';
			db.save(function(err){
				if(err) {
					response = {"error" : req.params.id+"create:save"};
					res.status(503).send(response);
					var consresp = "["+remote+":create] Error saving data";
				} else {
					response = {"id" : db.id,"create" : "New workitem created", "workitem" : db.workitem, "startp" : db.startp};
					var consresp = "["+remote+":create] "+db.workitem;
					res.json(response);
				}
				console.log(consresp);
			});
		} else {
			response = {"error" : "Access denied"};
			var consresp = "["+remote+":create] Unauthorized request";
			res.status(403).send(response);
			console.log(consresp)
		}
});

router.route("/workitem/wreq")
	.get(function(req,res){
		var freeLock = () => {};
		var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
		var response = {};
		var offer = moment();
		locks.lock("niilo22").then((free) => {
			if(wLstWorkers == "*" || wLstWorkers.indexOf(requestIP) >= 0) {
				freeLock = free;
				mongoOp.findOne({result: 'NONE', offer: '0'}).sort({ field: 'asc', _id: 1 }).exec(function(err,data){
				//const existOrNot = mongoOp.workitem.exists({result: 'NONE', offer: '0'})
				//if(existOrNot) {
					if(err) {
						response = {"error" : "Error fetching work"};
						res.status(503).send(response);
					} else if (mongoose.connection.db.collection('workitem').count() == 0) {
						respose = {"error" : "Empty database"};
						res.status(400).send(response);
					} else {
						if (mongoose.connection.db.collection('workitem.data.offer').count == 0) {
							response = {"error" : "Uninitialized db entry"};
							res.status(400).send(response);
						} else {
							data.offer = offer;
					        	data.save(function(err){
							if(err) {
								response = {"error" : req.params.id+"offer:save"};
								res.status(503).send(response);
								var consresp = "["+remote+":offer] Error saving data";
								console.log(consresp);
							} else {
								response = {"message" : data};
								var consresp = "["+remote+":offer] "+data.workitem;
								res.json(response);
								console.log(consresp);
							}
							});
						}
				        }
				});
			} else {
					response = {"error" : "Access denied"};
					res.status(403).send(response);
					console.log("[wreq] Unauthorized request @"+remote);
				}
			}).then(() => freeLock(), () => freeLock());
		})

	.post(function(req,res){
		var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
		var response = {};
		var timestamp = moment();
		if (wLstWorkers = "*" || wLstWorkers.indexOf(requestIP) >= 0) {
			response = {"wreq" : req.body.wuid};
			consresp = "["+remote+":register] Ok. wuID: "+req.body.wuid;
			res.json(response);
			console.log(consresp);
		} else {
			response = {"error" : "Access denied"};
			res.status(403).send(response);
			consresp("["+remote+":register] Unauthorized request");
			console.log(consresp);
		}
});

router.route("/adm/dropdb")
	.delete(function(req,res){
		var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
		if(wLstAdmins.indexOf(requestIP) >= 0) {
			var response = dropDb();
			console.log(`[_dropDb] All gone @`+requestIP);
			res.json(response);
		} else {
			var response = {"error" : "Access denied"};
			res.status(403).send(response);
			console.log("["+remote+":dropDb] Unauthorized request");
		}
	}
);
router.route("/adm/cleanexp")
	.delete(function(req,res){
		var response = {};
		var consresp = '';
                var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
                if(wLstAdmins.indexOf(requestIP) >= 0) {
			var result = cleanExp(remote);
		} else {
			response = {"error" : "Access denied"};
			consresp = "["+remote+"]:clean] Unauthorized requesti";
			res.status(403).send(response);
			console.log(consresp);
		}
	});

router.route("/workitem/:id")
	.get(function(req,res){
		var response = {};
		mongoOp.findById(req.params.id,function(err,data){
			if(err) {
				response = {"error" : req.params.id+"Error fetching work"};
				res.status(409).send(response);
			} else {
				response = {"message" : data};
				res.json(response);
			}
		});
	})
	.put(function(req,res){
		var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
		var response = {};
		if(wLstWorkers == '*' || wLstWorkers.indexOf(requestIP) >= 0 || wLstNodes == '*' || wLstNodes.indexOf(requestIP) >= 0) {
			mongoOp.findById(req.params.id,function(err,data){
				if(err || req.params.id == 'undefined') {
					response = {"error" : req.params.id+": Error fetching workitem"};
					res.status(503).send(response);
					console.log(`[error] Error fetching workitem`);
				} else if (data) {
					if(typeof req.body.result !== 'undefined' && req.body.result) {
						if(req.body.result !== 'working'){
							if(data.result) {
								data.result = req.body.result;
							}
							if(data.workitem) {
								data.workitem = req.body.workitem;
							}
							var endw = moment();
							if(data.startp){
								var mse = moment(endw).diff(moment(data.startp));
								var des = moment.duration(mse);
								var timealive = Math.floor(des.asHours()) + moment(mse).format(":mm:ss");
							} else {
								var timealive = 'no-startp';
							}
							if(data.startw){
								var ms = moment(endw).diff(moment(data.startw));
								var d = moment.duration(ms);
								var timeInWorker = Math.floor(d.asHours()) + moment(ms).format(":mm:ss");
							} else {
								var timeInWorker = 'no-startw';
							}
							data.duratp = timeInWorker;
							data.save(function(err){
								if(err) {
									response = {"error" : req.params.id+"Error saving data"};
									res.status(503).send(response);
									var consresp = "["+remote+":update] Error saving: "+req.params.id;
								} else {
									response = {"message" : req.params.id+" updated", "workitem" : req.params.workitem, "result" : req.params.result};
									if(data.result != 'expired') {
										var consresp = "["+remote+":result] timeWorking: "+timeInWorker+" result: "+data.result;
									} else {
										var consresp = "["+remote+":expire] timeExp: "+timealive;
									}
								console.log(consresp);
								res.json(response);
								}
							})
						} else {
							if(data.result == 'working' && req.body.result == 'working') {
								response = {"error" : "Workitem reserved"};
								res.status(409).send(response);
								console.log(`[Error] Workitem reserved`);
							} else {
								data.result = req.body.result;
								var startw = moment();
								data.startw = startw;
								var ms = moment(startw).diff(moment(data.startp));
								var d = moment.duration(ms);
								var totalwait = Math.floor(d.asHours()) + moment(ms).format(":mm:ss");
								data.duratw = totalwait;
								data.save(function(err){
									if(err) {
										response = {"error" : req.params.id+"Error saving data"};
										res.status(503).send(response);
										var consresp = "["+remote+":assign] Error saving: "+req.params.id;
									} else {
										response = {"message" : req.params.id, "result" : data.result, "startw" : data.startw};
										var consresp = "["+remote+":assign] timeWaiting: "+totalwait+" "+data.workitem+" "+data.result;
										res.json(response);
										console.log(consresp);
									}
								})
							}
						}
					} else {
						response = {"error" : "Invalid request"};
						res.status(400).send(response);
						console.log(remote+` Invalid request: `+req.body.result);
					}
				}
			})
		} else {
			response = {"error" : "Access denied"};
			res.status(403).send(response);
			console.log(`[`+remote+`:update] Unauthorized request`);
		}
	})
	.delete(function(req,res){
		var requestIP = req.connection.remoteAddress;
		var remote = resRIP(requestIP);
		if(wLstNodes == '*' || wLstNodes.indexOf(requestIP) >= 0) {
			mongoOp.findById(req.params.id,function(err,data){
				if(err) {
					response = {"error" : req.params.id+"findById"};
					res.status(400).send(response);
					console.log(`[`+remote+`:delete] Error findById`);
				} else {
					var finishtime = moment();
					var ms = moment(finishtime).diff(moment(data.startp));
					var d = moment.duration(ms);
					var totaltime = Math.floor(d.asHours()) + moment(ms).format(":mm:ss") || 'none';
					var tmpduratw = data.duratw || 'none';
					var tmpduratp = data.duratp || 'none';
					var tmpworkitem = data.workitem || 'none';
					var tmpresult = data.result || 'none';
					var respcons = "["+remote+":finish] timeTotal: "+totaltime+" timeWaiting: "+tmpduratw+" timeWorking: "+tmpduratp+" result:'"+tmpresult+"'";
					console.log(respcons);
					mongoOp.deleteOne({_id : req.params.id},function(err){
						if(err) {
							var response = {"error" : "Error deleting id: "+req.params.id};
							var respcons = "["+remote+":delete] Error deleting: "+req.params.id;
							res.status(503).send(response);
							console.log(respcons);
						} else {
							var response={"delete" : req.params.id};
							res.json(response);
						}
					});
				}
			});
		} else {
			response = {"error" : "Access denied"};
			res.status(403).send(response);
			console.log(`[_Delete] Unauthorized request @ `+remote);
		}
	})


app.use('/',router);
app.listen(port);
console.log(`\nclN22-manager v${version} [:${port}]\n`);
