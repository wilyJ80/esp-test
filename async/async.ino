//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* ssid = "VitaoNetwork";
const char* password = "senhaforte33";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "Not found");
}

void setup() {

	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.printf("WiFi Failed!\n");
		return;
	}

	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "text/html", R"rawliteral(
		<!DOCTYPE html>
		<html>
		<head>
		<meta charset="utf-8" />
		<title>Gamedev Canvas Workshop - lesson 10: finishing up</title>
		<style>* { padding: 0; margin: 0; } canvas { background: #eee; display: block; margin: 0 auto; }</style>
		</head>
		<body>

		<canvas id="myCanvas" width="480" height="320"></canvas>

		<script>
		var canvas = document.getElementById("myCanvas");
		var ctx = canvas.getContext("2d");
		var ballRadius = 10;
		var x = canvas.width/2;
		var y = canvas.height-30;
		var dx = 2;
		var dy = -2;
		var paddleHeight = 10;
		var paddleWidth = 75;
		var paddleX = (canvas.width-paddleWidth)/2;
		var rightPressed = false;
		var leftPressed = false;
		var brickRowCount = 5;
		var brickColumnCount = 3;
		var brickWidth = 75;
		var brickHeight = 20;
		var brickPadding = 10;
		var brickOffsetTop = 30;
		var brickOffsetLeft = 30;
		var score = 0;
		var lives = 3;

		var bricks = [];
		for(var c=0; c<brickColumnCount; c++) {
		bricks[c] = [];
		for(var r=0; r<brickRowCount; r++) {
		bricks[c][r] = { x: 0, y: 0, status: 1 };
		}
		}

		document.addEventListener("keydown", keyDownHandler, false);
		document.addEventListener("keyup", keyUpHandler, false);
		document.addEventListener("mousemove", mouseMoveHandler, false);

		function keyDownHandler(e) {
		if(e.code  == "ArrowRight") {
		rightPressed = true;
		}
		else if(e.code == 'ArrowLeft') {
		leftPressed = true;
		}
		}
		function keyUpHandler(e) {
		if(e.code == 'ArrowRight') {
		rightPressed = false;
		}
		else if(e.code == 'ArrowLeft') {
		leftPressed = false;
		}
		}
		function mouseMoveHandler(e) {
		var relativeX = e.clientX - canvas.offsetLeft;
		if(relativeX > 0 && relativeX < canvas.width) {
		paddleX = relativeX - paddleWidth/2;
		}
		}
		function collisionDetection() {
		for(var c=0; c<brickColumnCount; c++) {
		for(var r=0; r<brickRowCount; r++) {
		var b = bricks[c][r];
		if(b.status == 1) {
		if(x > b.x && x < b.x+brickWidth && y > b.y && y < b.y+brickHeight) {
		dy = -dy;
		b.status = 0;
		score++;
		if(score == brickRowCount*brickColumnCount) {
		alert("YOU WIN, CONGRATS!");
		document.location.reload();
		}
		}
		}
		}
		}
		}

		function drawBall() {
		ctx.beginPath();
		ctx.arc(x, y, ballRadius, 0, Math.PI*2);
		ctx.fillStyle = "#0095DD";
		ctx.fill();
		ctx.closePath();
		}
		function drawPaddle() {
		ctx.beginPath();
		ctx.rect(paddleX, canvas.height-paddleHeight, paddleWidth, paddleHeight);
		ctx.fillStyle = "#0095DD";
		ctx.fill();
		ctx.closePath();
		}
		function drawBricks() {
		for(var c=0; c<brickColumnCount; c++) {
		for(var r=0; r<brickRowCount; r++) {
		if(bricks[c][r].status == 1) {
		var brickX = (r*(brickWidth+brickPadding))+brickOffsetLeft;
		var brickY = (c*(brickHeight+brickPadding))+brickOffsetTop;
		bricks[c][r].x = brickX;
		bricks[c][r].y = brickY;
		ctx.beginPath();
		ctx.rect(brickX, brickY, brickWidth, brickHeight);
		ctx.fillStyle = "#0095DD";
		ctx.fill();
		ctx.closePath();
		}
		}
		}
		}
		function drawScore() {
		ctx.font = "16px Arial";
		ctx.fillStyle = "#0095DD";
		ctx.fillText("Score: "+score, 8, 20);
		}
		function drawLives() {
		ctx.font = "16px Arial";
		ctx.fillStyle = "#0095DD";
		ctx.fillText("Lives: "+lives, canvas.width-65, 20);
		}

		function draw() {
		ctx.clearRect(0, 0, canvas.width, canvas.height);
		drawBricks();
		drawBall();
		drawPaddle();
		drawScore();
		drawLives();
		collisionDetection();

		if(x + dx > canvas.width-ballRadius || x + dx < ballRadius) {
		dx = -dx;
		}
		if(y + dy < ballRadius) {
		dy = -dy;
		}
		else if(y + dy > canvas.height-ballRadius) {
		if(x > paddleX && x < paddleX + paddleWidth) {
		dy = -dy;
		}
		else {
		lives--;
		if(!lives) {
		alert("GAME OVER");
		document.location.reload();
		}
		else {
		x = canvas.width/2;
		y = canvas.height-30;
		dx = 2;
		dy = -2;
		paddleX = (canvas.width-paddleWidth)/2;
		}
		}
		}

		if(rightPressed && paddleX < canvas.width-paddleWidth) {
		paddleX += 7;
		}
		else if(leftPressed && paddleX > 0) {
		paddleX -= 7;
		}

		x += dx;
		y += dy;
		requestAnimationFrame(draw);
		}

		draw();
		</script>

		</body>
		</html>)rawliteral");
	});

	// Send a GET request to <IP>/get?message=<message>
	server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
		String message;
		if (request->hasParam(PARAM_MESSAGE)) {
			message = request->getParam(PARAM_MESSAGE)->value();
		} else {
			message = "No message sent";
		}
		request->send(200, "text/plain", "Hello, GET: " + message);
	});

	// Send a POST request to <IP>/post with a form field message set to <message>
	server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
		String message;
		if (request->hasParam(PARAM_MESSAGE, true)) {
			message = request->getParam(PARAM_MESSAGE, true)->value();
		} else {
			message = "No message sent";
		}
		request->send(200, "text/plain", "Hello, POST: " + message);
	});

	server.onNotFound(notFound);

	server.begin();
}

void loop() {
}
