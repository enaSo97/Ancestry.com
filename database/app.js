'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
	//console.log("in the function");
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
  let uploadFile = req.files.uploadFile;
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }
    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
	  console.log(path.join(__dirname+'/uploads/' + req.params.name));
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

/*app.post('/create', function(req, res){
	console.log(req.query.subName);
	console.log("int post function");
	res.redirect('/');
});*/

let sharedLib = ffi.Library('./libparser', {
	'FileInfo' : [ 'string' , ['string'] ],
	'getIndi' : [ 'string' , ['string'] ],
	'addIndiWrapper' : [ 'int' , ['string', 'string', 'string']  ],
	'createSimpleGEDCOM' : [ 'int' , ['string', 'string', 'string', 'string', 'string', 'string'] ],
	'returnDescendents' : [ 'string' , ['string','string', 'string' , 'int'] ],
	'returnAncestors' : [ 'string' , ['string','string', 'string' , 'int'] ],
	});
const mysql = require('mysql');
var connection;
	
app.get('/login', function(req,res){
	console.log("loging fucntion");
	connection = mysql.createConnection({
		host     : 'dursley.socs.uoguelph.ca',
		user     : req.query.userName,
		password : req.query.psd,
		database : req.query.name
	});
	connection.connect(function (err){
		if (err){
			console.log("connection error " + err);
			res.send("wrong");
		}else{
			connection.query("create table if not exists FILE (file_id int auto_increment,  file_Name varchar(60) not null,  source varchar(250), version varchar(10), encoding varchar(10) not null, sub_name varchar(62) not null, sub_addr varchar(256), num_individials int, num_families int, primary key(file_id) )", function (err, rows, fields) {
				if (err) console.log("Something went wrong. "+err);
				else{
					console.log("table created");
				}
			});
			connection.query("create table if not exists INDIVIDUAL (ind_id int auto_increment primary key, surname varchar(256) not null, given_name varchar(256) not null, sex varchar(1), fam_size int, source_file int, foreign key(source_file) references FILE(file_id) on delete cascade)", function (err, rows, fields) {
				if (err) console.log("Something went wrong. "+err);
				else{
					console.log("table created");
				}
			});
			console.log("connected");
			res.send("corrent");
		}
	});
});
// "INSERT INTO student (last_name, first_name, mark) VALUES ('Hugo','Victor','B+')", 
app.get('/delete', function(req,res){
	connection.query("delete from FILE");
	connection.query("ALTER TABLE FILE AUTO_INCREMENT = 1;");
	connection.query("delete from INDIVIDUAL");
	connection.query("ALTER TABLE INDIVIDUAL AUTO_INCREMENT = 1;");
	res.send("done");
});
app.get('/storeDB', function(req, res){
	//console.log(req.query.fileName);
	//connection.query("ALERT TABLE FILE AUTO_INCREMENT=1");
	let records = "INSERT INTO FILE (file_Name, source, version, encoding, sub_name, sub_addr, num_individials, num_families) VALUES (\'" 
				+ req.query.fileName + "\',\'" +  req.query.source + "\',\'" + req.query.version + "\',\'" + req.query.encode + "\',\'" + req.query.submit + "\',\'" + req.query.addr + "\',\'" + req.query.indi + "\',\'" + req.query.fam + "\')";
	//console.log("id " + fileID);
	var fileID = parseInt(req.query.ID) + 1;
	connection.query(records, function (err, rows, fields) {
        if (err) console.log("Something went wrong in store . "+err);
        //fileID = rows.insertId;
        //console.log("ID: " + fileID);
	});
	
	console.log(records);
	var text = sharedLib.getIndi('./uploads/' + req.query.fileName);
	console.log(text);
	
	var name;
	var last;
	var sex;
	var fam;
	var id = 1;				
	var obj = JSON.parse(text);
	if (obj.length == 0){
		console.log("no inidividuals");
		//$('<div>' + "Individuals does not exits!" + '</div>').appendTo('#view');
	}
	for (var key = 0; key < obj.length; key++){

		name = obj[key].givenName;
		last = obj[key].surname;
		sex = obj[key].sex;
		fam = obj[key].families;
		//$('#gedcomView tr:last').after('<tr>' + '<td>' + name + '</td>' + '<td>' + last + '</td>' + '<td>' + sex + '</td>' + '<td>' + fam + '</td>' + '</tr>');
		let toInsert = "INSERT INTO INDIVIDUAL (surname, given_name, sex, fam_size, source_file) VALUES (\'" 
		+ last + "\',\'"
		+ name + "\',\'"
		+ sex + "\',\'"
		+ fam + "\',\'"
		+ fileID + "\')";
		
		console.log(toInsert + "\n");
		
		connection.query(toInsert, function (err, rows, fields) {
			if (err) console.log("Something went wrong in store . "+err);
		});
	}
	
	res.send("succes");
    
});

app.get('/clearDB', function(req, res){
	console.log("in clear function ");
	let record = "DELETE FROM FILE";
	
	connection.query(record, function (err, rows, fields) {
        if (err) console.log("Something went wrong in clear. " +err);
        else{
			res.send("success");
		}
	});
    
});

app.get('/fileDB', function(req,res){
	let command = "SELECT COUNT(*) FROM FILE";
	connection.query(command, function (err, rows, fields) {
        if (err) console.log("Something went wrong in clear. " +err);
        for (let row of rows){
			console.log(row["COUNT(*)"]);
			res.send(row);
		}
        
        //res.send("d");
	});
	
});

app.get('/indiDB', function(req, res){
	let command2 = "SELECT COUNT(*) FROM INDIVIDUAL"
	connection.query(command2, function (err, rows, fields) {
        if (err) console.log("Something went wrong in clear. " +err);
        for (let row of rows){
			console.log(row["COUNT(*)"]);
			res.send(row);
		}
        //console.log(rows);
        
        //res.send("d");
	});
});

app.get('/helpQueryFILE', function (req,res){
	let command = "DESCRIBE FILE";
	connection.query(command, function(err, rows, fields){
		if (err) console.log("wrong in help file. " + err);
		
		res.send(rows);
	});
});

app.get('/helpQueryINDI', function (req,res){
	let command = "DESCRIBE INDIVIDUAL";
	connection.query(command, function(err, rows, fields){
		if (err) console.log("wrong in help indi. " + err);
		
		res.send(rows);
	});
});


app.get('/selectQuery', function (req,res){
	console.log(req.query.select);
	let info = req.query.select;
	var result = info.split(/[ ,]+/);
	console.log("printintf result " + result);
	
	console.log("before query");
	connection.query("SELECT " + info, function (err, rows, fields) {
        if (err){
			 console.log("Something went wrong in clear. " +err);
			 res.send(err);
		 }
        else{
			console.log(rows);
			var key = [];
			//console.log(Object.getOwnPropertyNames(rows));
			for (let row of rows){
				var thing = Object.getOwnPropertyNames(row);
				var i = 0;
				for (i in thing){
					console.log(row[thing[i]]);
				}
				//console.log(row[thing[0]]);
				key.push(row);
				
			}
			res.send(rows);
		}
	});
	//res.send("lit");
});

app.get("/query1", function(req, res){
	let command = "SELECT * FROM INDIVIDUAL ORDER BY surname";
	console.log("in query1 function");
	connection.query(command, function(err, rows, fields){
		if (err) console.log("something worng when getting Q1 " + err);
		console.log(rows);
		res.send(rows);
	});
	//res.send("wtf")
});

app.get("/query2", function(req,res){
	var fileName = req.query.fileName;
	console.log(fileName);
	var id;
	let command = "SELECT file_id FROM FILE WHERE file_name = \"" +fileName+"\"";
	console.log(command);
	connection.query(command, function(err, rows, fields){
		if (err) console.log("something worng when getting Q12" + err);
		res.send(rows);
	});
});

app.get('/query2.2', function(req, res){
	let command2 = "SELECT * FROM INDIVIDUAL WHERE source_file = " + req.query.file_id;
	console.log(command2);
	connection.query(command2, function(err, rows, fields){
		if (err) console.log("something worng when getting Q2 INDI" + err);
		res.send(rows);
		//console.log(rows);
	});
});

app.get('/query3', function(req, res){
	let command = "SELECT * FROM INDIVIDUAL WHERE sex = \"F\"";
	connection.query(command, function(err, rows, fields){
		if (err) console.log("something worng when getting Q3" + err);
		res.send(rows);
		//console.log(rows);
	});
});

app.get('/query4', function(req, res){
	let command = "SELECT * FROM INDIVIDUAL WHERE fam_size <" + 5 + " ORDER BY fam_size";
	connection.query(command, function(err, rows, fields){
		if (err) console.log("something worng when getting Q4" + err);
		res.send(rows);
		//console.log(rows);
	});
});

app.get('/query5', function(req, res){
	let command = "SELECT file_name, num_individials, num_families FROM FILE ORDER BY num_individials DESC";
	connection.query(command, function(err, rows, fields){
		if (err) console.log("something worng when getting Q5" + err);
		res.send(rows);
		//console.log(rows);
	});
});

const folder = './uploads/';
app.get('/getFile', function(req, res){
	//console.log(req.files);
	var string = "[";
	var n = 0;
	fs.readdir(folder, (err, files) => {
		files.forEach(file => {
			string += "{\"fileName\":\"" + file +"\"},"; 
		});
		string = string.slice(0,-1);
		string += "]";
		if (string == "]"){
			string = "[]";
		}
		console.log(string);
		res.send(string);
	});
});



app.get('/ListDesc', function(req,res){
	var file = "./uploads/" + req.query.fileName;
	var gen = req.query.gen;
	var first = req.query.givenName;
	var last = req.query.surname;
	console.log(file + " " + gen + " " + first + " " + last);
	var result = sharedLib.returnDescendents(file, first, last, gen);
	res.send(result);
});

app.get('/ListAnc', function(req,res){
	var file = "./uploads/" + req.query.fileName;
	var gen = req.query.gen;
	var first = req.query.givenName;
	var last = req.query.surname;
	var result = sharedLib.returnAncestors(file, first, last, gen);
	res.send(result);
});

app.get('/indiList', function(req, res){
	var file = req.query.fileName;
	var text = sharedLib.getIndi('./uploads/' + file);
	//console.log(text);
	console.log("|"+text+"|");
	res.send(text);
});

app.get('/insert', function(req, res){
	var file = "./uploads/" + req.query.fileName;
	//console.log(file);
	//console.log(req.query.givenName);
	var result = sharedLib.addIndiWrapper(file, req.query.givenName, req.query.surname);
	//console.log(file + "f: " + req.query.givenName + "l: ");
	//console.log(result);
	res.send('insert call back');
	
});

app.get('/getFileHead', function(req, res){
	var file = req.query.fileName;
	//console.log(req.query.givenName);
	var text = sharedLib.FileInfo('./uploads/' + file);
	//console.log(text);
	//console.log("wtf");
	
	res.send(text);
});

app.get('/create', function(req, res){
	var source = req.query.Source;
	var vers = req.query.Version;
	var encode = req.query.Encode;
	var name = req.query.Name
	var address = req.query.Address;
	var file = req.query.fileName;
	//console.log(source);
	//console.log(file);
	//console.log(encode);
	//console.log(vers);
	//console.log(name);
	//console.log(address);
	//console.log(givenName + surname);
	var result = sharedLib.createSimpleGEDCOM('./uploads/'+file, source, vers, encode, name, address)
	//console.log(result);
	if (result == 0){
		res.send("sucess");
	}else{
		res.send("fail");
	}
	//res.redirect('/');
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
