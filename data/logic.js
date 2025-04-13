let startEventsPending = false;										// Do we have a start event pending ?
let startEventsDelay = 0;											// Event delay before trying to reconnect
let inSetup = false;												// Are we in setup.htm?
let traceJava = false;												// Trace this java code
let messagesExtended = false;										// Is messages div extended?

// Display message on console and messages HTML element
function showMessage(message, displayOnBrowser = true, displayOnConsole = true) {
    if (displayOnConsole) {
	console.log(message);											// Display message on console
    }
	let messages = document.getElementById('messages');				// Messages from HTML document
	if (messages && displayOnBrowser) {								// If messages exists in document
		let messageElem = document.createElement('div');			// Create a div element
		messageElem.textContent = message;							// Set message into
		messages.prepend(messageElem);								// Add it at bottom of messages
	}
}

// Loads setting from JSON file. Print an error if field not found and loadHTML = true
function loadSettings(loadHtml) {
	const req = new XMLHttpRequest();
	// Executed when request state changec
	req.onreadystatechange = function() {
		// Check for end of request and status 200 (Ok)
		if (req.readyState == 4 && req.status == "200") {
			// Load JSON data
			jsonData = req.response;
			// Analyze all items in JSON file
			for (const key in jsonData) {
				if (setData(key, jsonData[key])) {
          if (loadHtml) {
            // There's no elements with that key in document
            showMessage("### Can't find "+key+" in document ###");
          }
        }
				if (key == "traceJava") {
					traceJava = jsonData[key] == true;
				}
			}
		}
		// Trace end of request with error
		if (req.readyState == 4 && req.status != "200") {
			showMessage(req.responseURL+" returned "+String(req.status)+"/"+req.responseText);
		}
	};
	// Ask for JSON file nammed settings.json
	req.responseType = "json";
	req.open("GET", "./settings.json");
    req.setRequestHeader("Cache-Control", "no-cache, no-store, max-age=0");
    req.setRequestHeader("Expires", "Tue, 01 Jan 1980 1:00:00 GMT");
    req.setRequestHeader("Pragma", "no-cache");
	req.send();
}

// Load just received data into document
function loadData(receivedData) {
  // Load JSON data
  const jsonData = JSON.parse(receivedData);
  // Analyze all items in JSON file
  for (const key in jsonData){
    if (setData(key, jsonData[key])) {
      // There's no elements with that key in document
      showMessage("### Can't find "+key+" in document ###");
    }
  }
}

// Set some data into document from received message
//  Return true if data not found (else false)
function setData(key, dataValue) {
    // Find element giving key in document
    element = document.getElementById(key);
    if (element != null) {
        // Is there any check?
        if (element.type == "checkbox") {                           // .type is loaded only for "INPUT xxx"
        element.checked = (String(dataValue).toLowerCase() == "true");
        } else if (element.nodeName == "DIV" && key.substring(0,4).toLowerCase() == "hide") {
            console.log(key+" is "+ String(dataValue));
            element.hidden = (String(dataValue).toLowerCase() == "true");
      // For other elements
      } else {
        // If content is number but not integer (float)
        if (typeof(dataValue) == "number" && !Number.isInteger(dataValue)) {
          // Round it to 2 decimals
          dataValue = roundOf(dataValue, 2);
        }
        // Is there any value,
        if (element.type == "text" || element.type == "number" || element.type == "button") {
            element.value = dataValue;
        } else {
          //	Else, load text
          element.innerText = dataValue;
        }
      }
    } else {
        element = document.getElementsByName(key);
        if (element != null) {
            for (let i = 0; i < element.length; i++) {
                element[i].checked = (element[i].value == dataValue);
              }
        } else {
            return true;
        }
    }
    return false;
}
  
// Init all things, with a flag to tell if we're in setup or not
function initAll(inSetupContext) {
	inSetup = inSetupContext;
	loadSettings(inSetup);
	initMessages();
	startEvents(!inSetup);
}

// Init message <div>
function initMessages() {
	const divMessage = document.getElementById("messages");														// Look for "message" div
	if (divMessage) {
		divMessage.ondblclick = function () {																	// Called when double click on messages div
			messagesExtended = !messagesExtended;																// Invert message extended
			document.getElementById("messages").className = (messagesExtended ? "largeScroll" : "smallScroll"); // Set heigth 10 times initial one if extended
		};
	}
}

// Connect events and define associated functions
function startEvents(getDataEvents){
	const es = new EventSource('/events');							// Connect
	startEventsPending = false;										// Clear start pending
	es.onopen = function(e) {										// Executed when connected
		showMessage("# Events opened #", traceJava);				// Just log
		startEventsDelay = 0;										// We tried to open, reset start delay
	};
	es.onerror = function(e) {										// Executed in case of error
		if (e.target.readyState == EventSource.CLOSED) {			// If state is "closed"
			showMessage("### Events closed ("+e.target.readyState+") ###"); // Log
			if (!startEventsPending) {								// If no start pending
				setTimeout(startEvents, startEventsDelay * 1000);	// Ask for a delayed start 
				startEventsPending = true;							// Set start pending
				if (startEventsDelay < 10) {						// Increase delay up to 10 sec
					startEventsDelay++;
				}
			}
		}
	};
	es.addEventListener('settings', (e) => {						// Executed when receiving a "settings" event
		if (e.data) {
  	  console.log("Settings: "+ e.data);						// Log
  	  loadSettings(inSetup);									// Reload setting giving saved inSetup flag
		}
	});
	es.addEventListener('info', (e) => {							// Executed when receiving an "info" event
		if (e.data) {
            showMessage(e.data, true, false);                       // Set message error
		}
	});
	es.addEventListener('error', (e) => {							// Executed when receiving an "error" event
		if (e.data) {
		  showMessage("*** "+e.data+" ***");						// Set message error
		}
	});
    if (getDataEvents) {
      es.addEventListener('data', (e) => {						// Executed when receiving an "error" event
        if (e.data) {
          loadData(e.data);	                    				// Load received data
        }
    });    
    }
}

// Executed when a value is changed by user
function changed(object, value="") {
	showMessage("# Set changed "+object.id+":"+object.name+":"+object.value+" #", traceJava);
	const req = new XMLHttpRequest();
  if (value !== "") {
            req.open("GET", location.origin+'/changed/'+object.id+"/"+value); // Value is sent when given
  } else if (object.id.substring(0,5) == "trace" || object.id.substring(0,6) == "enable" || object.id.substring(0,3) == "led") {
  		req.open("GET", location.origin+'/changed/'+object.id+"/"+object.checked);		// .checked is send for checkboxes
  } else if (object.name !== "") {
  		req.open("GET", location.origin+'/changed/'+object.name+"/"+object.value);		// Use name if defined
  } else {
  		req.open("GET", location.origin+'/changed/'+object.id+"/"+object.value);		// .id else
  }
	req.onreadystatechange = function() {
		// Trace end of request with error
		if (req.readyState == 4 && req.status != "200") {
			showMessage(req.responseURL+" returned "+String(req.status)+"/"+req.responseText);
		}
	};
	req.send();
}

// Sends a trigger to server, trace errors
function trigger(url) {
	showMessage("# Send message to "+url+" #", traceJava);
	const req = new XMLHttpRequest();
	req.open("GET", location.origin+url);
	req.onreadystatechange = function() {
		// Trace end of request with error
		if (req.readyState == 4 && req.status != "200") {
			showMessage(req.responseURL+" returned "+String(req.status)+"/"+req.responseText);
		}
	};
	req.send();
}

// Rounds a float f to p decimals
function roundOf(n, p) {
	const n1 = n * Math.pow(10, p + 1);
	const n2 = Math.floor(n1 / 10);
	if (n1 >= (n2 * 10 + 5)) {
		return (n2 + 1) / Math.pow(10, p);
	}
	return n2 / Math.pow(10, p);
}

// Load all keys and values from an object
var getKeysValues = function(obj) {
    var keysValues = [];
    for (var key in obj){
       keysValues.push(key+"="+String(obj[key]));
    }
    return keysValues;
}