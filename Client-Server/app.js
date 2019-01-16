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
