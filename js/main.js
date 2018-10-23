var actual_JSON = init();

function getDate(){
  var d = new Date();
  return d.toString();
}

function init() {
 loadJSON(function(response) {
    actual_JSON = JSON.parse(response);

    // console.log(actual_JSON);
 });
 return actual_JSON;
}

function loadJSON(callback) {
    var xobj = new XMLHttpRequest();

    xobj.overrideMimeType("application/json");
    xobj.open('GET', "data.json", true); // Replace 'my_data' with the path to your file
    xobj.onreadystatechange = function () {
          if (xobj.readyState == 4 && xobj.status == "200") {
            // Required use of an anonymous callback as .open will NOT return a value but simply returns undefined in asynchronous mode
            callback(xobj.responseText);
          }
    };
    xobj.send(null);
 }


function submitFunction() {
    var i = 0;
    actual_JSON = actual_JSON;

while(i < actual_JSON.length){

  var table = document.getElementById("myTable");
  var row = table.insertRow(myTable.length);
  if(actual_JSON[i].Latitude < 28.0  && actual_JSON[i].Longitude != 0.0){
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);
    var cell5 = row.insertCell(4);
    cell1.innerHTML = document.getElementById("fname").value;
    cell2.innerHTML = actual_JSON[i].Datetime;
    cell3.innerHTML = actual_JSON[i].Latitude;
    cell4.innerHTML = actual_JSON[i].Longitude;
    // cell6.innerHTML = actual_JSON[i].Nitrate;
    }
    i++;
  }
}


//////////////////////////////////////////////
function exportTableToCSV() {

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



function initMap() {

  // actual_JSON = init();

  var center = {lat: Number(actual_JSON[0].Latitude), lng: Number(actual_JSON[0].Longitude)};

  var map = new google.maps.Map(document.getElementById('map'), {
    center: center,
    zoom: 17,
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
        infowindow.setContent("Altitude: " + actual_JSON[count].Altitude);
        infowindow.open(map, marker);
      }
    })(marker, count));
  }
}


jQuery(document).ready(function() { //this fuction is for the back to top button
    var offset = 220;
    var duration = 500;
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
