#pragma once

const char HTML_MAIN1[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>{device_id}</title><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\"><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\"></head><body><style>body {background-color: #ffdd99;background-image: url(\"data:image/svg+xml,%3Csvg xmlns=\'http://www.w3.org/2000/svg\' width=\'100%25\'%3E%3Cdefs%3E%3ClinearGradient id=\'a\' gradientUnits=\'userSpaceOnUse\' x1=\'0\' x2=\'0\' y1=\'0\' y2=\'100%25\' gradientTransform=\'rotate(240)\'%3E%3Cstop offset=\'0\' stop-color=\'%23ffdd99\'/%3E%3Cstop offset=\'1\' stop-color=\'%234FE\'/%3E%3C/linearGradient%3E%3Cpattern patternUnits=\'userSpaceOnUse\' id=\'b\' width=\'540\' height=\'450\' x=\'0\' y=\'0\' viewBox=\'0 0 1080 900\'%3E%3Cg fill-opacity=\'0.1\'%3E%3Cpolygon fill=\'%23444\' points=\'90 150 0 300 180 300\'/%3E%3Cpolygon points=\'90 150 180 0 0 0\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'270 150 360 0 180 0\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'450 150 360 300 540 300\'/%3E%3Cpolygon fill=\'%23999\' points=\'450 150 540 0 360 0\'/%3E%3Cpolygon points=\'630 150 540 300 720 300\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'630 150 720 0 540 0\'/%3E%3Cpolygon fill=\'%23444\' points=\'810 150 720 300 900 300\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'810 150 900 0 720 0\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'990 150 900 300 1080 300\'/%3E%3Cpolygon fill=\'%23444\' points=\'990 150 1080 0 900 0\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'90 450 0 600 180 600\'/%3E%3Cpolygon points=\'90 450 180 300 0 300\'/%3E%3Cpolygon fill=\'%23666\' points=\'270 450 180 600 360 600\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'270 450 360 300 180 300\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'450 450 360 600 540 600\'/%3E%3Cpolygon fill=\'%23999\' points=\'450 450 540 300 360 300\'/%3E%3Cpolygon fill=\'%23999\' points=\'630 450 540 600 720 600\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'630 450 720 300 540 300\'/%3E%3Cpolygon points=\'810 450 720 600 900 600\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'810 450 900 300 720 300\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'990 450 900 600 1080 600\'/%3E%3Cpolygon fill=\'%23444\' points=\'990 450 1080 300 900 300\'/%3E%3Cpolygon fill=\'%23222\' points=\'90 750 0 900 180 900\'/%3E%3Cpolygon points=\'270 750 180 900 360 900\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'270 750 360 600 180 600\'/%3E%3Cpolygon points=\'450 750 540 600 360 600\'/%3E%3Cpolygon points=\'630 750 540 900 720 900\'/%3E%3Cpolygon fill=\'%23444\' points=\'630 750 720 600 540 600\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'810 750 720 900 900 900\'/%3E%3Cpolygon fill=\'%23666\' points=\'810 750 900 600 720 600\'/%3E%3Cpolygon fill=\'%23999\' points=\'990 750 900 900 1080 900\'/%3E%3Cpolygon fill=\'%23999\' points=\'180 0 90 150 270 150\'/%3E%3Cpolygon fill=\'%23444\' points=\'360 0 270 150 450 150\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'540 0 450 150 630 150\'/%3E%3Cpolygon points=\'900 0 810 150 990 150\'/%3E%3Cpolygon fill=\'%23222\' points=\'0 300 -90 450 90 450\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'0 300 90 150 -90 150\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'180 300 90 450 270 450\'/%3E%3Cpolygon fill=\'%23666\' points=\'180 300 270 150 90 150\'/%3E%3Cpolygon fill=\'%23222\' points=\'360 300 270 450 450 450\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'360 300 450 150 270 150\'/%3E%3Cpolygon fill=\'%23444\' points=\'540 300 450 450 630 450\'/%3E%3Cpolygon fill=\'%23222\' points=\'540 300 630 150 450 150\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'720 300 630 450 810 450\'/%3E%3Cpolygon fill=\'%23666\' points=\'720 300 810 150 630 150\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'900 300 810 450 990 450\'/%3E%3Cpolygon fill=\'%23999\' points=\'900 300 990 150 810 150\'/%3E%3Cpolygon points=\'0 600 -90 750 90 750\'/%3E%3Cpolygon fill=\'%23666\' points=\'0 600 90 450 -90 450\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'180 600 90 750 270 750\'/%3E%3Cpolygon fill=\'%23444\' points=\'180 600 270 450 90 450\'/%3E%3Cpolygon fill=\'%23444\' points=\'360 600 270 750 450 750\'/%3E%3Cpolygon fill=\'%23999\' points=\'360 600 450 450 270 450\'/%3E%3Cpolygon fill=\'%23666\' points=\'540 600 630 450 450 450\'/%3E%3Cpolygon fill=\'%23222\' points=\'720 600 630 750 810 750\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'900 600 810 750 990 750\'/%3E%3Cpolygon fill=\'%23222\' points=\'900 600 990 450 810 450\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'0 900 90 750 -90 750\'/%3E%3Cpolygon fill=\'%23444\' points=\'180 900 270 750 90 750\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'360 900 450 750 270 750\'/%3E%3Cpolygon fill=\'%23AAA\' points=\'540 900 630 750 450 750\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'720 900 810 750 630 750\'/%3E%3Cpolygon fill=\'%23222\' points=\'900 900 990 750 810 750\'/%3E%3Cpolygon fill=\'%23222\' points=\'1080 300 990 450 1170 450\'/%3E%3Cpolygon fill=\'%23FFF\' points=\'1080 300 1170 150 990 150\'/%3E%3Cpolygon points=\'1080 600 990 750 1170 750\'/%3E%3Cpolygon fill=\'%23666\' points=\'1080 600 1170 450 990 450\'/%3E%3Cpolygon fill=\'%23DDD\' points=\'1080 900 1170 750 990 750\'/%3E%3C/g%3E%3C/pattern%3E%3C/defs%3E%3Crect x=\'0\' y=\'0\' fill=\'url(%23a)\' width=\'100%25\' height=\'100%25\'/%3E%3Crect x=\'0\' y=\'0\' fill=\'url(%23b)\' width=\'100%25\' height=\'100%25\'/%3E%3C/svg%3E\");background-attachment: fixed;background-size: cover;background-position: center;padding: 20px;}.boxshadow {box-shadow: 1px 2px 4px rgba(0, 0, 0, .5);background-color: #FFF;border-style: solid;border-color: #ddd;border-width: 1px;}.panel {box-shadow: 1px 2px 4px rgba(0, 0, 0, .5);}</style><script type=\"application/javascript\">function sendValue(input) {$.ajax(input.dataset.url,{\'data\': $(input).closest(\"form\").find(\"input\").val(),\'type\': \'POST\',\'processData\': false,\'contentType\': \'application/json\'});}setInterval(function() {$(\"[data-ajaxurl]\").each(function(index) {var ele = $(this);$.getJSON( ele.data(\"ajaxurl\"), function (data) {var eleValue =  $(ele.find(\"#value\")[0]);if(data.value.toUpperCase() == \"OPEN\") {eleValue.html(\"<span class=\\\"label label-success\\\">\" + data.value + \"</span>\");} else if(data.value.toUpperCase() == \"CLOSE\") {eleValue.html(\"<span class=\\\"label label-danger\\\">\" + data.value + \"</span>\");} else {eleValue.html(data.value);}var eleTime = $(ele.find(\".panel-footer span\")[0]);eleTime.html(data.updated);});});}, 1000);</script><div class=\"container\"><nav class=\"navbar navbar-default\"><div class=\"container-fluid\"><div class=\"navbar-header\"><button type=\"button\" class=\"navbar-toggle collapsed\" data-toggle=\"collapse\" data-target=\"#bs-example-navbar-collapse-1\" aria-expanded=\"false\"><span class=\"sr-only\">Toggle navigation</span><span class=\"icon-bar\"></span><span class=\"icon-bar\"></span><span class=\"icon-bar\"></span></button><a class=\"navbar-brand\" href=\"#\">{device_id}</a></div><div class=\"collapse navbar-collapse\" id=\"bs-example-navbar-collapse-1\"><ul class=\"nav navbar-nav navbar-right\"><li role=\"presentation\" class=\"active\"><a href=\"#sensors\" aria-controls=\"sensors\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-dashboard\" aria-hidden=\"true\"></span> Sensors</a></li><li role=\"presentation\"><a href=\"#devices\" aria-controls=\"devices\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-pencil\" aria-hidden=\"true\"></span> Devices</a></li><li role=\"presentation\"><a href=\"#config\" aria-controls=\"config\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-pencil\" aria-hidden=\"true\"></span> Config</a></li><li role=\"presentation\"><a href=\"#about\" aria-controls=\"about\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-book\" aria-hidden=\"true\"></span> About</a></li></ul></div></div></nav><div class=\"tab-content panel panel-default\"><div role=\"tabpanel\" class=\"tab-pane active\" id=\"sensors\"><br /><div class=\"container-fluid\"><div class=\"row\" style=\"min-height: 600px\">";
const char HTML_SENSOR[] PROGMEM = "<div class=\"col-md-4\"><div class=\"panel panel-{color}\" data-ajaxurl=\"{path}\"><div class=\"panel-heading\"><strong>{name}</strong></div><ul class=\"list-group\">{input}{output}</ul><div class=\"panel-footer\" style=\"padding: 5px 15px; font-size: 85%\"><strong>Last updated:</strong> <span>{last_updated}</span></div></div></div>";
const char HTML_SENSOR_INPUT[] PROGMEM = "<li class=\"list-group-item\"><form><div class=\"input-group\"><input type=\"text\" class=\"form-control\" placeholder=\"value\"><span class=\"input-group-btn\"><button class=\"btn btn-default\" type=\"button\" data-url=\"{path}\" onclick=\"sendValue(this);\">Send</button></span></div></form></li>";
const char HTML_SENSNOR_INPUT[] PROGMEM = "";
const char HTML_SENSOR_OUTPUT[] PROGMEM = "<li class=\"list-group-item\" style=\"font-size: 24px;\"><span id=\"value\">{value}</span></li>";
const char HTML_VALUE_ON[] PROGMEM = "<span class=\"label label-success\">{value}</span>";
const char HTML_VALUE_OFF[] PROGMEM = "<span class=\"label label-danger\">{value}</span>";
const char HTML_MAIN2[] PROGMEM = "</div></div></div><div role=\"tabpanel\" class=\"tab-pane\" id=\"devices\"><div class=\"container-fluid\"><div class=\"row\" style=\"min-height: 600px\"><div class=\"col-md-12\"><table class=\"table table-striped\"><thead><tr><th>ID</th><th>Name</th><th>Online</th><th>IP</th><th>Last seen</th><th>Link</th></tr></thead>";
const char HTML_DEVICES[] PROGMEM = "<tr><td>{id}</td><td>{name}</td><td>{online}</td><td>{ip}</td><td>{lastUpdated}</td><td><a href=\"http://{ip}\">Link</a></td></tr>";
const char HTML_MAIN3[] PROGMEM = "</table></div></div></div></div><div role=\"tabpanel\" class=\"tab-pane\" id=\"config\"><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-md-12\"><form class=\"form-horizontal\" action=\"/save\" method=\"POST\">";
const char HTML_CONFIG_HEADER[] PROGMEM = "<br /><h3>General</h3><hr/>";
const char HTML_CONFIG_ENTRY[] PROGMEM = "<div class=\"form-group\"><label for=\"{key}\" class=\"col-sm-2 control-label\">{key}</label><div class=\"col-sm-8\"><input type=\"{type}\" class=\"form-control\" name=\"{key}\" placeholder=\"\" value=\"{value}\"></div></div>";
const char HTML_MAIN4[] PROGMEM = "<hr/><div class=\"form-group\"><div class=\"col-sm-offset-9 col-sm-2\"><button type=\"submit\" class=\"btn btn-default\">Save</button></div></div></form></div></div></div></div><div role=\"tabpanel\" class=\"tab-pane\" id=\"about\"><br /><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-md-12\"><h1>About</h1><p>Device is using EasyMqtt version 0.4</p><dl class=\"dl-horizontal\"><dt>Device Id</dt><dd>{device_id}</dd><dt>Topic</dt><dd>{topic}</dd></dl><hr /><h3>MQTT API:</h3><ul class=\"list-group\">";
const char HTML_MAIN5[] PROGMEM = "</ul><hr /><h3>Rest API:</h3><ul class=\"list-group\">";
const char HTML_API_DOC[] PROGMEM = "<li class=\"list-group-item\">{path}</li>";
const char HTML_MAIN6[] PROGMEM = "</ul></div></div></div></div><div class=\"panel-footer\" style=\"padding: 5px 15px; font-size: 85%\"><center>Powered by <a href=\"https://github.com/bloft/EasyMqtt.git\">EasyMqtt</a></center></div></div></div><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script><script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script></body></html>";
