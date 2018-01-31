#ifndef html_h
#define html_h

const char HTML_MAIN1[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>Device Info - {device_id}</title><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\"><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\"></head><body style=\"background-color: #F0F0F0\"><script type=\"application/javascript\">function sendValue(input) {$.ajax({url: input.dataset.url, method: \"POST\", data: $(input).closest(\"form\").find(\"input\").val()}).complete(function (data, textStatus, jqXHR) {alert(textStatus);});}setInterval(function() {$(\"[data-ajaxurl]\").each(function(index) {var ele = $(this);$.ajax(ele.data(\"ajaxurl\")).success(function (data) {if(data.toUpperCase() == \"OPEN\") {ele.html(\"<span class=\\\"label label-success\\\">\" + data + \"</span>\");} else if(data.toUpperCase() == \"CLOSE\") {ele.html(\"<span class=\\\"label label-danger\\\">\" + data + \"</span>\");} else {ele.html(data);}});});}, 1000);</script><div class=\"container\"><h1 style=\"background-color: #FFF;padding: 6px 14px;border: 1px solid #e1e1e8;border-radius: 4px;\">Device Info - {device_id}</h1><ul class=\"nav nav-tabs\" role=\"tablist\"><li role=\"presentation\" class=\"active\"><a href=\"#devices\" aria-controls=\"devices\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-dashboard\" aria-hidden=\"true\"></span> Devices</a></li><li role=\"presentation\"><a href=\"#config\" aria-controls=\"config\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-pencil\" aria-hidden=\"true\"></span> Config</a></li><li role=\"presentation\"><a href=\"#about\" aria-controls=\"about\" role=\"tab\" data-toggle=\"tab\"><span class=\"glyphicon glyphicon-book\" aria-hidden=\"true\"></span> About</a></li></ul><div class=\"tab-content\" style=\"border-style: solid; border-color: #ddd; border-width: 0px 1px 1px 1px; background-color: #FFF\"><div role=\"tabpanel\" class=\"tab-pane active\" id=\"devices\"><br /><div class=\"container-fluid\"><div class=\"row\">";
const char HTML_SENSOR[] PROGMEM = "<div class=\"col-md-4\"><div class=\"panel panel-info\"><div class=\"panel-heading\"><strong>{name}</strong></div><div class=\"panel-body\" data-ajaxurl=\"{path}\" style=\"font-size: 24px;\">{value}</div></div></div>";
const char HTML_INPUT[] PROGMEM = "<div class=\"col-md-4\"><div class=\"panel panel-success\"><div class=\"panel-heading\"><strong>{name}</strong></div><div class=\"panel-body\" style=\"font-size: 24px;\"><form><div class=\"input-group\"><input type=\"text\" class=\"form-control\" placeholder=\"value\"><span class=\"input-group-btn\"><button class=\"btn btn-default\" type=\"button\" data-url=\"{path}\" onclick=\"sendValue(this);\">Send</button></span></div></form></div></div></div>";
const char HTML_VALUE_ON[] PROGMEM = "<span class=\"label label-success\">{value}</span>";
const char HTML_VALUE_OFF[] PROGMEM = "<span class=\"label label-danger\">{value}</span>";
const char HTML_MAIN2[] PROGMEM = "</div></div></div><div role=\"tabpanel\" class=\"tab-pane\" id=\"config\"><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-md-12\"><form class=\"form-horizontal\" action=\"/save\" method=\"POST\">";
const char HTML_CONFIG_HEADER[] PROGMEM = "<br /><h3>{title}</h3><hr/>";
const char HTML_CONFIG_ENTRY[] PROGMEM = "<div class=\"form-group\"><label for=\"{key}\" class=\"col-sm-2 control-label\">{key}</label><div class=\"col-sm-8\"><input type=\"{type}\" class=\"form-control\" id=\"{key}\" placeholder=\"\" value=\"{value}\"></div></div>";
const char HTML_MAIN3[] PROGMEM = "<hr/><div class=\"form-group\"><div class=\"col-sm-offset-9 col-sm-2\"><button type=\"submit\" class=\"btn btn-default\">Save</button></div></div></form></div></div></div></div><div role=\"tabpanel\" class=\"tab-pane\" id=\"about\"><br /><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-md-12\"><h1>About</h1><p>Device is using EasyMqtt version 0.1</p><dl class=\"dl-horizontal\"><dt>Device Id</dt><dd>{device_id}</dd><dt>Topic</dt><dd>{topic}</dd></dl><hr /><h3>MQTT API:</h3><ul class=\"list-group\">";
const char HTML_MAIN4[] PROGMEM = "</ul><hr /><h3>Rest API:</h3><ul class=\"list-group\">";
const char HTML_API_DOC[] PROGMEM = "<li class=\"list-group-item\">{path}</li>";
const char HTML_MAIN5[] PROGMEM = "</ul></div></div></div></div></div></div><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script><script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script></body></html>";

#endif
