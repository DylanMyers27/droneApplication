//fetch json
var actual_JSON = function (){
  fetch('data.json')
    .then(data => data.json())
    .then(json => actual_JSON = json)
    .catch(err => console.log(err));
}();

//this function will reload the page every 10 seconds if it detects new data in json
var previous = null;
var current = null;
setInterval(function getData(){
     $.getJSON("data.json", function(json) {
         current = JSON.stringify(json);
         if (previous && current && previous !== current) {
             console.log('refresh');
             location.reload();
         }
         previous = current;
         console.table(current);
     });
 },5000);

//gets current date not used as of now
 function getDate(){
   var d = new Date();
   return d.toString();
 }

//this is to generate a table of all the results on button press
function submitFunction() {
var i = 0;
var table = document.getElementById("myTable");

  while(i < actual_JSON.length){
    var row = table.insertRow(myTable.length);
      if(actual_JSON[i].Latitude < 28.0  && actual_JSON[i].Longitude != 0.0){
        var cell1 = row.insertCell(0);
        var cell2 = row.insertCell(1);
        var cell3 = row.insertCell(2);
        var cell4 = row.insertCell(3);
        var cell5 = row.insertCell(4);
        cell1.textContent = document.getElementById("fname").value;
        cell2.textContent = actual_JSON[i].Datetime;
        cell3.textContent = actual_JSON[i].Latitude;
        cell4.textContent = actual_JSON[i].Longitude;
        cell5.innerHTML = actual_JSON[i].Altitude;
      }
    i++;
  }
}



//////////////////////////////////////////////
function exportTableToCSV() { //does what function name says

    var csv = [];

    var rows = document.querySelectorAll("table tr");

    for (var i = 0; i < rows.length; i++) {
        var row = [], cols = rows[i].querySelectorAll("td, th");

        for (var j = 0; j < cols.length; j++)
            row.push(cols[j].innerText);

        csv.push(row.join(","));
    }

    // Download CSV file
    downloadCSV(csv.join("\n"), 'actual_JSON.csv');
}

function downloadCSV(csv, filename) {
    var csvFile;
    var downloadLink;

    // CSV file
    csvFile = new Blob([csv], {type: "text/csv"});

    // Download link
    downloadLink = document.createElement("a");

    // File name
    downloadLink.download = filename;
    // Create a link to the file
    downloadLink.href = window.URL.createObjectURL(csvFile);

    // Hide download link
    downloadLink.style.display = "none";

    // Add the link to DOM
    document.body.appendChild(downloadLink);

    // Click download link
    downloadLink.click();
}
//////////////////////////////////////////
// Pass in sensor data inside where it says 'Everglades 1'
//Also provide a way to feed in new long and latitude data



function initMap() { // initializes the google map

  var center = {lat: Number(actual_JSON[0].Latitude), lng: Number(actual_JSON[0].Longitude)};

  var map = new google.maps.Map(document.getElementById('map'), {
    center: center,
    zoom: 18,
    mapTypeId: google.maps.MapTypeId.HYBRID
  });
  var infowindow =  new google.maps.InfoWindow;
  var marker, count;
  for (count = 0; count < actual_JSON.length; count++) {
    marker = new google.maps.Marker({
      position: new google.maps.LatLng(Number(actual_JSON[count].Latitude),Number(actual_JSON[count].Longitude)),
      map: map
    });
  google.maps.event.addListener(marker, 'click', (function (marker, count) {
      return function () {
        infowindow.setContent("Nitrate Level: " + actual_JSON[count].Altitude + " mg/L");
        infowindow.open(map, marker);
      }
    })(marker, count));
  }
}




jQuery(document).ready(function() { //this fuction is for the back to top button
    var offset = 220;
    var duration = 100;
    jQuery(window).scroll(function() {
        if (jQuery(this).scrollTop() > offset) {
            jQuery('.back-to-top').fadeIn(duration);
        } else {
            jQuery('.back-to-top').fadeOut(duration);
        }
    });

    jQuery('.back-to-top').click(function(event) {
        event.preventDefault();
        jQuery('html, body').animate({scrollTop: 0}, duration);
        return false;
    })
});

// had loading issues so this fetches data seperately
$.get("data.json", function (data) { //this is the dynamic CanvasJS chart

var dps = []; // dataPoints
var chart = new CanvasJS.Chart("chartContainer", {
    title :{
        text: "Nitrate Concentrations"
    },
    axisY: {
        includeZero: false,
        title: "Nitrate Levels",
    },
    axisX:{
        title: "Number of Samples"
    },
    data: [{
        type: "line",
        dataPoints: dps,
    }]
});


var xVal = 1;
var yVal = 5.0;
var updateInterval = 2500;
var dataLength = 5; // number of dataPoints visible at any point

var updateChart = function (count) {

    count = count || 1;

    for (var j = 0; j < count; j++) {
        yVal = Number(actual_JSON[j].Altitude);
        dps.push({
            x: xVal,
            y: yVal
        });
        xVal++;
    }

    if (dps.length < dataLength) {
        dps.shift();
    }

    chart.render();
};

updateChart(dataLength);
setInterval(function(){updateChart()}, updateInterval);
});
