// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {

    $.ajax({
		type: 'get',
		dataType: 'text',
		url: '/getFile',
		success: function(data){
			var obj = JSON.parse(data);
			console.log(obj.length);
			if (obj.length == 0){
				$('<div align="center">' + "No files uplodaed!" + '</div>').appendTo('#log');
				document.getElementById("clearData").disabled = true;
				if (document.getElementById("clearData").disabled == true){
					alert("No files in the server,\nClear Database Button disabled");
				}
			}
			var head_objs = [];
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
				//var head_objs = [];
				var counter = 0;
				var file_array;
				$.ajax({
					type: 'get',
					async: false,
					dataType: 'text',
					url: '/getFileHead',
					data: {fileName : file},
					success: function(stuff){
						if (stuff != "Fail"){
							var head = JSON.parse(stuff);
							console.log("head " + head);
							head_objs.push(head);
							console.log("files " + head_objs.length);
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
							var x = document.getElementById("selectDrop");
							var n = document.getElementById('ancFileDrop');
							var option = document.createElement("option");
							var option2 = document.createElement("option");
							var option3 = document.createElement("option");
							var option4 = document.createElement("option");
							var option5 = document.createElement("option");
							var y = document.getElementById("indiDrop");
							var z = document.getElementById("descFileDrop");
							var k = document.getElementById("query2Drop");
							option.text = file;
							option2.text = file;
							option3.text = file;
							option4.text = file;
							option5.text = file;
							x.add(option);
							y.add(option2);
							z.add(option3);
							n.add(option4);
							k.add(option5);

						}
						$('#fileLog tr:last').after('<tr>' + '<td>' + path + '</td>' + '<td>' + source + '</td>' + '<td>' + version + '</td>' + '<td>' + encode + '</td>' + '<td>' + submit + '</td>' + '<td>' + address + '</td>' + '<td>' + indi + '</td>' + '<td>' + fam + '</td>' +  '</tr>');
					
					},
					fail: function(error){
						console.log("error?");
						console.log(error);
					}
					
				});
				
			}
			//console.log("files in the server " + head_objs.length);
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
			var indis = [];
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
							
							var obj = JSON.parse(thing);
							if (obj.length == 0){
								 $('<div>' + "Individuals does not exits!" + '</div>').appendTo('#view');
							}
							for (index in obj){

								name = obj[index].givenName;
								last = obj[index].surname;
								sex = obj[index].sex;
								fam = obj[index].families;
								$('#gedcomView tr:last').after('<tr>' + '<td>' + name + '</td>' + '<td>' + last + '</td>' + '<td>' + sex + '</td>' + '<td>' + fam + '</td>' + '</tr>');
								
							}
						},
						fail: function(error){
							console.log(error);
						}
					});
				});
			});
			
			/*********Storing files to data base******************************/
			$("#store").click(function(e){
				console.log("store clicked");
				$.ajax({
						type: 'get',
						dataType: 'text',
						async: false,
						url: '\delete',
						success: function(data){
						},
						fail: function(err){
							console.log(err);
						}
				});
				console.log("length " + head_objs.length);
				for (key in head_objs){
					var address;
					console.log(head_objs[key]);
					if (head_objs[key].address == undefined){
						address="";
					}else{
						address = head_objs[key].address;
					}
					//console.log("adsfasdf " + obj[key].fileName);
					//address = head_objs[key].address;
					$.ajax({
						type: 'get',
						dataType: 'text',
						async: false,
						url: '\storeDB',
						data: {
							ID : key,
							fileName : obj[key].fileName,
							source : head_objs[key].source,
							version : head_objs[key].gedcVersion,
							encode : head_objs[key].encoding,
							submit : head_objs[key].submitterName,
							addr : address,
							indi : head_objs[key].individuals,
							fam : head_objs[key].families
						},
						success: function(thing){
							console.log("store done");
						},
						fail: function(err){
							console.log(err);
						}
					});
				}
				$.ajax({
					type: 'get',
					dataType: 'json',
					async: false,
					url: '/fileDB',
					success: function(thing){
						console.log(thing);
						$.ajax({
							type: 'get',
							dataType: 'json',
							url: '/indiDB',
							success: function(data){
								console.log(data);
								$("#demo").append("Database has " +thing["COUNT(*)"]+ " files and " +data["COUNT(*)"]+ " individuals.<br/>");
							},
							fail: function(err){
								console.log(err);
							}
						});
					},
					fail: function(err){
						console.log(err);
					}
				});
				//console.log(head_objs);
			});
			
			/******clearing the database ****************************/
			/*$("#clearData").click(function(e){
				console.log("clear clicked");
				
				$.ajax({
						type: 'get',
						async: false,
						url: '/clearDB',
						success: function(thing){
							console.log("insert success");
						},
						fail: function(err){
							console.log(err);
						}
					});
				
			});*/
			
			$('#clearData').click(function(e){
				if (document.getElementById("clearData").disabled == true){
					alert("Clear Button disabled");
				}
				console.log("clear clicked");
				$.ajax({
					type: 'get',
					async: false,
					url: '/clearDB',
					success: function(thing){
						console.log("insert success");
					},
					fail: function(err){
						console.log(err);
					}
				});
				$.ajax({
					type: 'get',
					dataType: 'json',
					async: false,
					url: '/fileDB',
					success: function(thing){
						console.log(thing);
						$.ajax({
							type: 'get',
							dataType: 'json',
							url: '/indiDB',
							success: function(data){
								console.log(data);
								$("#demo").append("Database has " +thing["COUNT(*)"]+ " files and " +data["COUNT(*)"]+ " individuals.<br/>");
							},
							fail: function(err){
								console.log(err);
							}
						});
					},
					fail: function(err){
						console.log(err);
					}
				});
			});
			$('#displayDB').click(function(e){
				console.log("wwawadisplay clicked");
				$.ajax({
					type: 'get',
					dataType: 'json',
					async: false,
					url: '/fileDB',
					success: function(thing){
						console.log(thing);
						$.ajax({
							type: 'get',
							dataType: 'json',
							url: '/indiDB',
							success: function(data){
								console.log(data);
								$("#demo").append("Database has " +thing["COUNT(*)"]+ " files and " +data["COUNT(*)"]+ " individuals.<br/>");
							},
							fail: function(err){
								console.log(err);
							}
						});
					},
					fail: function(err){
						console.log(err);
					}
				});
			});
			
			/***********Help Query***********************************/
			$("#help").click(function(e){
				e.preventDefault();
				$(".all2").show();
				console.log("help clikced");
				$.ajax({
					type: 'get',
					dataType: 'json',
					url: '/helpQueryFILE',
					success: function(data){
						var check = 0;
						$('#fileHelp').empty();
						for (let row of data){
							var thing = Object.getOwnPropertyNames(row);
							if (check == 0){
								var head = "<tr>";
								for (j in thing){
									head += "<th>" + thing[j] + "</th>";
								}
								head += "</tr>";
								$('#fileHelp').append(head);
								$('#fileHelp tr').addClass("colorChange");
								$('#fileHelp th').addClass("subStyle");
							}
							var adding = "<tr>";
							var i = 0;
							for (i in thing){
								adding += "<td>" + row[thing[i]] + "</td>";
								
								console.log(row[thing[i]]);
							}
							adding += "</tr>";
							
							$('#fileHelp').append(adding);
							$('#fileHelp tr').addClass("colorChange");
							$('#fileHelp td').addClass("subStyle");
							//console.log(row[thing[0]]);
							//key.push(row);
							check++;
							
						}
					},
					fail: function(err){
						console.log(err);
					}
				});
				$.ajax({
					type: 'get',
					dataType: 'json',
					url: '/helpQueryINDI',
					success: function(data){
						var check = 0;
						$('#indiHelp').empty();
						for (let row of data){
							var thing = Object.getOwnPropertyNames(row);
							if (check == 0){
								var head = "<tr>";
								for (j in thing){
									head += "<th>" + thing[j] + "</th>";
								}
								head += "</tr>";
								$('#indiHelp').append(head);
								$('#indiHelp tr').addClass("colorChange");
								$('#indiHelp th').addClass("subStyle");
							}
							var adding = "<tr>";
							var i = 0;
							for (i in thing){
								adding += "<td>" + row[thing[i]] + "</td>";
								
								console.log(row[thing[i]]);
							}
							adding += "</tr>";
							
							$('#indiHelp').append(adding);
							$('#indiHelp tr').addClass("colorChange");
							$('#indiHelp td').addClass("subStyle");
							//console.log(row[thing[0]]);
							//key.push(row);
							check++;
							
						}
					},
					fail: function(err){
						console.log(err);
					}
				});
			});
			
			/***********Select Query********************************/
			$("#select").click(function(e){
				e.preventDefault();
				console.log(document.getElementById('userSelect').value);
				var string = document.getElementById('userSelect').value;
				if (string.length == 0){
					alert("Enter the command please");
				}else{
					$.ajax({
					type: 'get',
					dataType: 'json',
					async: false,
					url: '/selectQuery',
					data: {
						select : document.getElementById('userSelect').value
					},
					success: function(data){
							console.log("data"  + data);
							if (typeof data.sqlMessage == "undefined"){
								var check = 0;
								$('#selectTable').empty();
								for (let row of data){
									var thing = Object.getOwnPropertyNames(row);
									if (check == 0){
										var head = "<tr>";
										for (j in thing){
											head += "<th>" + thing[j] + "</th>";
										}
										head += "</tr>";
										$('#selectTable').append(head);
										$('#selectTable tr').addClass("colorChange");
										$('#selectTable th').addClass("subStyle");
									}
									var adding = "<tr>";
									var i = 0;
									for (i in thing){
										adding += "<td>" + row[thing[i]] + "</td>";
										
										console.log(row[thing[i]]);
									}
									adding += "</tr>";
									
									$('#selectTable').append(adding);
									$('#selectTable tr').addClass("colorChange");
									$('#selectTable td').addClass("subStyle");
									//console.log(row[thing[0]]);
									//key.push(row);
									check++;
									
								}
							
								$("#h3Text").html("Select Query");
								//document.getElementById('userSelect').attr('placeholder');
							}else{
								alert("Wrong command, try again");
							}
							
						},
						fail: function(err){
							console.log(err);
						}
					});
				}
			});
			/*********************Query 1 list all indi sorted by last *************/
			$("#Q1").click(function(e){
				console.log("Q1 clicked");
				$.ajax({
					type: 'get',
					dataType: 'json',
					//async: false,
					url: '/query1',
					success: function(data){
						console.log("wtf " + data);
						var check = 0;
						$('#selectTable').empty();
						for (let row of data){
							var thing = Object.getOwnPropertyNames(row);
							if (check == 0){
								var head = "<tr>";
								for (j in thing){
									head += "<th>" + thing[j] + "</th>";
								}
								head += "</tr>";
								$('#selectTable').append(head);
								$('#selectTable tr').addClass("colorChange");
								$('#selectTable th').addClass("subStyle");
							}
							var adding = "<tr>";
							var i = 0;
							for (i in thing){
								adding += "<td>" + row[thing[i]] + "</td>";
								
								console.log(row[thing[i]]);
							}
							adding += "</tr>";
							
							$('#selectTable').append(adding);
							$('#selectTable tr').addClass("colorChange");
							$('#selectTable td').addClass("subStyle");
							//console.log(row[thing[0]]);
							//key.push(row);
							check++;
							
						}
						$("#h3Text").html("list all individuals sorted by surname");
					},
					fail: function(err){
						console.log(err);
					}
				});
			});
			
			/****************Query2 choose file ********/
			$("#Q2").click(function(e){
				console.log("Q2 clicked");
				$("#Q2subdiv").show();
				$("#query2Drop").on('change', function(){
					var val = $(this).val();
					$.ajax({
						type: 'get',
						dataType: 'json',
						url: '/query2',
						data: {fileName : val},
						success: function(data){
							var id;
							//console.log(data);
							for (let row of data){
								id = row.file_id;
								console.log("id " + id);
							}
							$.ajax({
								type: 'get',
								dataType: 'json',
								url: '/query2.2',
								data: {file_id : id},
								success: function(data){
									//console.log(data);
									var check = 0;
									$('#selectTable').empty();
										for (let row of data){
											var thing = Object.getOwnPropertyNames(row);
											if (check == 0){
												var head = "<tr>";
												for (j in thing){
													head += "<th>" + thing[j] + "</th>";
												}
												head += "</tr>";
												$('#selectTable').append(head);
												$('#selectTable tr').addClass("colorChange");
												$('#selectTable th').addClass("subStyle");
											}
											var adding = "<tr>";
											var i = 0;
											for (i in thing){
												adding += "<td>" + row[thing[i]] + "</td>";
												
												console.log(row[thing[i]]);
											}
											adding += "</tr>";
											
											$('#selectTable').append(adding);
											$('#selectTable tr').addClass("colorChange");
											$('#selectTable td').addClass("subStyle");
											//console.log(row[thing[0]]);
											//key.push(row);
											check++;
											
										}
									
								},
								fail: function(err){
									console.log(err);
								}
							});
							$("#h3Text").html("listing individuals in specific file");
						},
						fail: function(err){
							console.log(err);
						}
					});
				});
			});
			/******************Query3 Find all the female in the individula ***/
			$("#Q3").click(function(e){
				console.log("Q3 clikced");
				$.ajax({
					type: 'get',
					dataType: 'json',
					url: '/query3',
					success: function(data){
						var check = 0;
							$('#selectTable').empty();
							for (let row of data){
								var thing = Object.getOwnPropertyNames(row);
								if (check == 0){
									var head = "<tr>";
									for (j in thing){
										head += "<th>" + thing[j] + "</th>";
									}
									head += "</tr>";
									$('#selectTable').append(head);
									$('#selectTable tr').addClass("colorChange");
									$('#selectTable th').addClass("subStyle");
								}
								var adding = "<tr>";
								var i = 0;
								for (i in thing){
									adding += "<td>" + row[thing[i]] + "</td>";
									
									console.log(row[thing[i]]);
								}
								adding += "</tr>";
								
								$('#selectTable').append(adding);
								$('#selectTable tr').addClass("colorChange");
								$('#selectTable td').addClass("subStyle");
								//console.log(row[thing[0]]);
								//key.push(row);
								check++;
										
							}
							$("#h3Text").html("Find all the female in the individuals");
					},
					fail: function(data){
						console.log(data);
					}
				});
			});
		},
		fail: function(error){
			console.log(error);
		}
	});
	
	/*****************Query4 find indiviauls with fam size < 5***/
	$("#Q4").click(function(e){
		console.log("Q4 clikced");
		$.ajax({
			type: 'get',
			dataType: 'json',
			url: '/query4',
			success: function(data){
				var check = 0;
					$('#selectTable').empty();
					for (let row of data){
						var thing = Object.getOwnPropertyNames(row);
						if (check == 0){
							var head = "<tr>";
							for (j in thing){
								head += "<th>" + thing[j] + "</th>";
							}
							head += "</tr>";
							$('#selectTable').append(head);
							$('#selectTable tr').addClass("colorChange");
							$('#selectTable th').addClass("subStyle");
						}
						var adding = "<tr>";
						var i = 0;
						for (i in thing){
							adding += "<td>" + row[thing[i]] + "</td>";
							
							console.log(row[thing[i]]);
						}
						adding += "</tr>";
						
						$('#selectTable').append(adding);
						$('#selectTable tr').addClass("colorChange");
						$('#selectTable td').addClass("subStyle");
						//console.log(row[thing[0]]);
						//key.push(row);
						check++;
								
					}
					$("#h3Text").html("Find indiviauls with fam size < 5");
			},
			fail: function(data){
				console.log(data);
			}
		});
	});
	
	$("#Q5").click(function(e){
		console.log("Q5 clikced");
		$.ajax({
			type: 'get',
			dataType: 'json',
			url: '/query5',
			success: function(data){
				var check = 0;
					$('#selectTable').empty();
					for (let row of data){
						var thing = Object.getOwnPropertyNames(row);
						if (check == 0){
							var head = "<tr>";
							for (j in thing){
								head += "<th>" + thing[j] + "</th>";
							}
							head += "</tr>";
							$('#selectTable').append(head);
							$('#selectTable tr').addClass("colorChange");
							$('#selectTable th').addClass("subStyle");
						}
						var adding = "<tr>";
						var i = 0;
						for (i in thing){
							adding += "<td>" + row[thing[i]] + "</td>";
							
							console.log(row[thing[i]]);
						}
						adding += "</tr>";
						
						$('#selectTable').append(adding);
						$('#selectTable tr').addClass("colorChange");
						$('#selectTable td').addClass("subStyle");
						//console.log(row[thing[0]]);
						//key.push(row);
						check++;
								
					}
					$("#h3Text").html("list the files ordered by num_individuals in desc order");
			},
			fail: function(data){
				console.log(data);
			}
		});
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
	/***Login for Data Base**************/
	
	$("#loginForm").submit(function(e){
		e.preventDefault();
		console.log("log in clicked");
		var user = document.getElementById('user').value;
		var pw = document.getElementById('pass').value;
		var data = document.getElementById('database').value;
		$.ajax({
			type: 'get',
			dataType: 'text',
			//async: false,
			url: '/login',
			data: {
				userName : user,
				psd : pw,
				name : data
			},
			success: function(thing){
				if (thing == "wrong"){
					alert("Wrong username or password, please try again");
				}else{
					alert("Connection Successfull! ")
				}
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
