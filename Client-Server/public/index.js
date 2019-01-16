// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

    $.ajax({
		type: 'get',
		dataType: 'text',
		url: '/getFile',
		success: function(data){
			//console.log("dum");
			//console.log(data.length);
			var obj = JSON.parse(data);
			console.log(obj.length);
			if (obj.length == 0){
				$('<div align="center">' + "No files uplodaed!" + '</div>').appendTo('#log');
			}
			for (key in obj){
				var file = obj[key].fileName;
				var path = file.link("/uploads/" + file);
				console.log(path);
				var source = "";
				var version = "";
				var encode = "";
				var submit = "";
				var address = "";
				var indi = "";
				var fam = "";
				
				$.ajax({
					type: 'get',
					async: false,
					dataType: 'text',
					url: '/getFileHead',
					data: {fileName : file},
					success: function(stuff){
						if (stuff != "Fail"){
							var head = JSON.parse(stuff);
			
							//console.log("calling c function");
							document.getElementById('FILE_LOG').innerHTML = source;
							source = head.source;
							version = head.gedcVersion;
							encode = head.encoding;
							submit = head.submitterName;
							indi = head.individuals;
							fam = head.families;
							if (head.address == undefined){
								address="";
							}else{
								address = head.address;
							}
							address = head.address;
							console.log(stuff);
							//$('#fileLog tr:last').after('<tr>' + '<td>' + path + '</td>' + '<td>' + source + '</td>' + '<td>' + version + '</td>' + '<td>' + encode + '</td>' + '<td>' + submit + '</td>' + '<td>' + address + '</td>' + '<td>' + indi + '</td>' + '<td>' + fam + '</td>' +  '</tr>');
							var x = document.getElementById("selectDrop");
							var n = document.getElementById('ancFileDrop');
							var option = document.createElement("option");
							var option2 = document.createElement("option");
							var option3 = document.createElement("option");
							var option4 = document.createElement("option");
							var y = document.getElementById("indiDrop");
							var z = document.getElementById("descFileDrop");
							option.text = file;
							option2.text = file;
							option3.text = file;
							option4.text = file;
							x.add(option);
							y.add(option2);
							z.add(option3);
							n.add(option4);
							//console.log("calling c function");
						}
						$('#fileLog tr:last').after('<tr>' + '<td>' + path + '</td>' + '<td>' + source + '</td>' + '<td>' + version + '</td>' + '<td>' + encode + '</td>' + '<td>' + submit + '</td>' + '<td>' + address + '</td>' + '<td>' + indi + '</td>' + '<td>' + fam + '</td>' +  '</tr>');
					
					},
					fail: function(error){
						console.log("error?");
						console.log(error);
					}
					
				});
				
			}
			var elements = document.getElementsByTagName('a');
			console.log(elements);
			for(var i = 0, len = elements.length; i < len; i++){
				console.log("my Suff" + elements[i].href);
				elements[i].onclick = function(){
					console.log("here");
					alert("clicked");
					$("#demo").append(" successfully donwloaded<br/>");
					
				}
			}
			$(function(){
				$('#selectDrop').on('change', function(){
					var val = $(this).val();
					console.log(val);
					$("#view").empty();
					$("#gedcomView").find("tr:not(:first)").remove();
					$.ajax({
						type: 'get',
						dataType: 'text',
						async: false,
						url: '\indiList',
						data: {fileName : val},
						success: function(thing){
							var name;
							var last;
							var sex;
							var fam;
							//console.log(thing);
							//console.log("got here");
							//var indiSelect = document.getElementById("indiDropList");
							var obj = JSON.parse(thing);
							if (obj.length == 0){
								 $('<div>' + "Individuals does not exits!" + '</div>').appendTo('#view');
							}
							for (index in obj){
								//console.log(obj[index].givenName);
								name = obj[index].givenName;
								last = obj[index].surname;
								sex = obj[index].sex;
								fam = obj[index].families;
								$('#gedcomView tr:last').after('<tr>' + '<td>' + name + '</td>' + '<td>' + last + '</td>' + '<td>' + sex + '</td>' + '<td>' + fam + '</td>' + '</tr>');
								/*var option = document.createElement("option");
								option.text = name + "," + last;
								indiSelect.add(option);*/
							}
							//console.log(obj);
						},
						fail: function(error){
							console.log(error);
						}
					});
				});
			});
			//console.log(data);
		},
		fail: function(error){
			console.log(error);
		}
	});
	
	$("#clear").click(function(e){
		//document.getElementById("status").reset();
		$("#status").empty();
		//localStorage.clear();
	});
	
	$('#indiDrop').on('change', function(){
		var val = $(this).val();
		
		$('#Individual').submit(function(e){
			e.preventDefault();
			var first = document.getElementById("name").value;
			var last = document.getElementById("last").value;
			console.log(first);
			console.log(last);
			$.ajax({
				//async: false;,
				type: 'get',
				dataType: 'text',
				url: '/insert',
				data: {
					fileName : val,
					givenName : first, 
					surname : last
					},
				success: function(data){
					console.log("sucecs");
					alert("Individual added to " + val + "\nps. refresh the page to update the file log and view panel");
					$("#demo").append(first + " " + last + "successfully added to " + val + "<br/z>");
					//localStorage.setItem("status","uploaded");
				},
				fail: function(error){
					console.log(error);
				}
				
			});
			//console.log("clicked add");
		});
		//localStorage.setItem(status, first + " " + last "got added to " + val + "file");
		//localStorage.setItem("status","uploaded");
	});
	
	$("#create").submit(function(e){
		e.preventDefault();
		var file = document.getElementById("fileInput").value;
		var source = $("#hSource").text();
		var encode = $("#encode").text();
		var version = $("#vers").text();
		var name = document.getElementById("subName").value;
		var address = document.getElementById("subAddress").value;
		var sub = file.substring(file.length-4, file.length);
		if (sub == ".ged"){
			
			//localStorage.setItem('message', file + "successfully uploaded");
			//document.getElementById('demo').innerHTML = file + " successfully created and uploaded<br/>";
			$("#demo").append(file + " successfully created and uploaded<br/>");
			console.log(sub);
			console.log(source + encode + version + name + address);
			document.getElementById("create").reset();
			$.ajax({
				type: 'get',
				dataType: 'text',
				url: '/create',
				data: {
					fileName: file,
					Source : source,
					Encode : encode,
					Version : version,
					Name : name,
					Address : address,
				},
				success: function(data){
					if (data == "fail"){
						alert("File not created successfully");
					}else{
						alert("Successfully created simple GEDCOM!");
						$("#fileLog").find("tr:not(:first)").remove();
						$('#selectDrop').find('option:not(:first)').remove();
						$('#indiDrop').find('option:not(:first)').remove();
						$('#ancFileDrop').find('option:not(:first)').remove();
						$('#descFileDrop').find('option:not(:first)').remove();
						$.ajax({
							type: 'get',
							dataType: 'text',
							url: '/getFile',
							success: function(data){
								//console.log("dum");
								//console.log(data.length);
								var obj = JSON.parse(data);
								console.log(obj);
								for (key in obj){
									var file = obj[key].fileName;
									var path = file.link("/uploads/" + file);
									var source = "";
									var version = "";
									var encode = "";
									var submit = "";
									var address = "";
									var indi = "";
									var fam = "";
									//$("#fileLog").find("tr:not(:first)").remove();
									$.ajax({
										type: 'get',
										async: false,
										dataType: 'text',
										url: '/getFileHead',
										data: {fileName : file},
										success: function(stuff){
											if (stuff != "Fail"){
												var head = JSON.parse(stuff);
												//console.log("calling c function");
												document.getElementById('FILE_LOG').innerHTML = source;
												source = head.source;
												version = head.gedcVersion;
												encode = head.encoding;
												submit = head.submitterName;
												indi = head.individuals;
												fam = head.families;
												if (head.address == undefined){
													address="";
												}else{
													address = head.address;
												}
												address = head.address;
												console.log("these stuff " + stuff);
												$('#fileLog tr:last').after('<tr>' + '<td>' + path + '</td>' + '<td>' + source + '</td>' + '<td>' + version + '</td>' + '<td>' + encode + '</td>' + '<td>' + submit + '</td>' + '<td>' + address + '</td>' + '<td>' + indi + '</td>' + '<td>' + fam + '</td>' +  '</tr>');
												var x = document.getElementById("selectDrop");
												var n = document.getElementById('ancFileDrop');
												var option = document.createElement("option");
												var option2 = document.createElement("option");
												var option3 = document.createElement("option");
												var option4 = document.createElement("option");
												var y = document.getElementById("indiDrop");
												var z = document.getElementById("descFileDrop");
												option.text = file;
												option2.text = file;
												option3.text = file;
												option4.text = file;
												x.add(option);
												y.add(option2);
												z.add(option3);
												n.add(option4);
												//console.log("calling c function");
											}
											//$('#fileLog tr:last').after('<tr>' + '<td>' + path + '</td>' + '<td>' + source + '</td>' + '<td>' + version + '</td>' + '<td>' + encode + '</td>' + '<td>' + submit + '</td>' + '<td>' + address + '</td>' + '<td>' + indi + '</td>' + '<td>' + fam + '</td>' +  '</tr>');
										},
										fail: function(error){
											console.log("error?");
											console.log(error);
										}
										
									});
									
								}
								$(function(){
									$('#selectDrop').on('change', function(){
										var val = $(this).val();
										//$('#selectDrop').find('option:not(:first)').remove();
										console.log(val);
										/*var Parent = document.getElementById('gedcomView');
										while(Parent.hasChildNodes()){
											Parent.removeChild(Parent.firstChild);
										}*/
										$("#gedcomView").find("tr:not(:first)").remove();
										$.ajax({
											type: 'get',
											dataType: 'text',
											async: false,
											url: '\indiList',
											data: {fileName : val},
											success: function(thing){
												var name;
												var last;
												var sex;
												var fam;
												//console.log(thing);
												//console.log("got here");
												//var indiSelect = document.getElementById("indiDropList");
												var obj = JSON.parse(thing);
												for (index in obj){
													//console.log(obj[index].givenName);
													name = obj[index].givenName;
													last = obj[index].surname;
													sex = obj[index].sex;
													fam = obj[index].families;
													$('#gedcomView tr:last').after('<tr>' + '<td>' + name + '</td>' + '<td>' + last + '</td>' + '<td>' + sex + '</td>' + '<td>' + fam + '</td>' + '</tr>');
													/*var option = document.createElement("option");
													option.text = name + "," + last;
													indiSelect.add(option);*/
												}
												//console.log(obj);
											},
											fail: function(error){
												console.log(error);
											}
										});
									});
								});
								//console.log(data);
							},
							fail: function(error){
								console.log(error);
							}
						});
						//location.reload();
					}
				},
				fail: function(error){
					console.log(error);
				}	
			});
		}else{
			$("#demo").append("Create GEDCOM file name must be .ged : syntax error<br\>");
			alert("File Name Format WRONG!");
		}
	});
	/*************DECS and ANC************/
	var fullName;
	var selectedFile;
	$('#descFileDrop').on('change', function(){
		//$("#descDisplay").find("tr:not(:first)").remove();
		$('#indiDropList').find('option:not(:first)').remove();
		var val = $(this).val();
		console.log(val);
		selectedFile = val
		$.ajax({
			type: 'get',
			dataType: 'text',
			async: false,
			url: '/indiList',
			data: {fileName : val},
			success: function(thing){
				var indiSelect = document.getElementById("indiDropList");
				var obj = JSON.parse(thing);
				for (key in obj){
					console.log(obj[key]);
					var option = document.createElement("option");
					//fullName = obj[key].givenName + "-" + obj[key].surname;
					option.text = obj[key].givenName + "-" + obj[key].surname;
					indiSelect.add(option);
				}
				/*$('#indiListDrop').on('change', function(){
			
					var val2 = $(this).val();
					console.log(val2);
				});*/
			},
			fail: function(error){
				console.log(error);
			}
		});
	});
	$('#indiDropList').on('change', function(){
		$("#descDisplay").find("tr:not(:first)").remove();
		$('#Message').empty();
		var val2 = $(this).val();
		fullName = val2;
		//console.log("check" + val);
		console.log(val2);
		console.log("file " + selectedFile);
	});
	$("#getDesc").submit(function(e){
		e.preventDefault();
		var maxGen = document.getElementById("generation").value;
		//console.log(maxGen);
		var first = fullName.split("-")[0];
		var last = fullName.split("-")[1];
		console.log("fisrt: " + first+ "|" + " last: " + last+"|");
		console.log("what?");
		$.ajax({
			type: 'get',
			dataType: 'text',
			//async: false,
			url: '/ListDesc',
			data: {
				fileName : selectedFile,
				givenName : first,
				surname : last,
				gen : maxGen
			},
			success: function(thing){
				//console.log(thing);
				var master = JSON.parse(thing);
				var num  = 1;
				if (master.length == 0){
					 $('<div>' + "Descendents does not exits!" + '</div>').appendTo('#Message');
					//alert("no people");
				}
				for (key in master){
					var generation = "";
					var list = master[key];
					//var num = 1;
					for (index in list){
					
						console.log("gen" + key);
						console.log(list[index]);
						generation += list[index].givenName + " " + list[index].surname + ", ";
					}
					$('#descDisplay tr:last').after('<tr>' + '<td>' + num + '</td>' + '<td>' + generation + '</td>' + '</tr>');
					
					num = num + 1;
					//console.log(list);
				}
				$("#demo").append("Getting descendent for " + first + " " + last + " <br/>");
			},
			fail: function(error){
				console.log(error);
			}
		});
	});
	
	/***getting Ancestors******************/
	var fullNameANC;
	var selectedFileANC;
	$('#ancFileDrop').on('change', function(){
		//$("#descDisplay").find("tr:not(:first)").remove();
		$('#indiDropList2').find('option:not(:first)').remove();
		var val = $(this).val();
		selectedFileANC = val
		$.ajax({
			type: 'get',
			dataType: 'text',
			async: false,
			url: '/indiList',
			data: {fileName : val},
			success: function(thing){
				var indiSelect = document.getElementById("indiDropList2");
				var obj = JSON.parse(thing);
				for (key in obj){
					console.log(obj[key]);
					var option = document.createElement("option");
					//fullName = obj[key].givenName + "-" + obj[key].surname;
					option.text = obj[key].givenName + "-" + obj[key].surname;
					indiSelect.add(option);
				}
			},
			fail: function(error){
				console.log(error);
			}
		});
	});
	$('#indiDropList2').on('change', function(){
		$("#ancDisplay").find("tr:not(:first)").remove();
		$('#Message2').empty();
		var val2 = $(this).val();
		fullNameANC = val2;
		//console.log("check" + val);
		console.log(val2);
		console.log("file " + selectedFileANC);
	});
	$("#getAnc").submit(function(e){
		e.preventDefault();
		var maxGen = document.getElementById("generation2").value;
		//console.log(maxGen);
		var first = fullNameANC.split("-")[0];
		var last = fullNameANC.split("-")[1];
		console.log("fisrt: " + first+ "|" + " last: " + last+"|");
		console.log("what?");
		$.ajax({
			type: 'get',
			dataType: 'text',
			//async: false,
			url: '/ListAnc',
			data: {
				fileName : selectedFileANC,
				givenName : first,
				surname : last,
				gen : maxGen
			},
			success: function(thing){
				//console.log(thing);
				var master = JSON.parse(thing);
				if (master.length == 0){
					 $('<div class="alertMes">' + "Ancestor does not exits!" + '</div>').appendTo('#Message2');
					//alert("no people");
				}
				var num  = 1;
				for (key in master){
					var generation = "";
					var list = master[key];
					//var num = 1;
					for (index in list){
					
						console.log("gen" + key);
						console.log(list[index]);
						generation += list[index].givenName + " " + list[index].surname + ", ";
					}
					$('#ancDisplay tr:last').after('<tr>' + '<td>' + num + '</td>' + '<td>' + generation + '</td>' + '</tr>');
					
					num = num + 1;
					//console.log(list);
				}
				$("#demo").append("Getting ancestor for " + first + " " + last + " <br/>");
			},
			fail: function(error){
				console.log(error);
			}
		});
	});

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
 
    /*var elements = document.getElementsByTagName('a');
    console.log(elements);
    for(var i = 0, len = elements.length; i < len; i++){
		console.log("my Suff" + element[i].value);
		elements[i].onclick = function(){
			console.log("here");
			$("#demo").append("Get" + elements[i].value + " <br/>");
			
		}
	}*/
});

function showUpload(id){
	var x = document.getElementById(id);
	if(x.style.display === "block"){
		x.style.display = "none";
	}else{
		x.style.display = "block";
	}
}

function uploadFunction(){
    var fileInput = document.getElementById('myFile');   
    
    var filename = fileInput.files[0].name;
    if (typeof filename !== 'undefined') {
		// Now we know that foo is defined, we are good to go.
		document.getElementById("demo").innerHTML = "file " + " " + filename + " uploaded";
	}
    //document.getElementById("demo").innerHTML = "file " + " " + filename + " uploaded";
}
// Get the modal
var modal = document.getElementById('id01');

// When the user clicks anywhere outside of the modal, close it
window.onclick = function(event) {
    if (event.target == modal) {
        modal.style.display = "none";
    }
}

function fileValidation(){
    var fileInput = document.getElementById('FILE');
    var filePath = fileInput.value;
    var allowedExtensions = /(\.ged)$/i;
    if(!allowedExtensions.exec(filePath)){
        alert('Please upload file having extensions .ged only.');
        fileInput.value = '';
        return false;
    }
}
