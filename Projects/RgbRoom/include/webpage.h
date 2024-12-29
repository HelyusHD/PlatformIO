#ifndef MY_WEBPAGE
#define MY_WEBPAGE

	const char webpage[] PROGMEM = R"rawliteral(
	<!DOCTYPE html>
	<html>
	<head>
	<title>ESP32 Webserver</title>
	<style>
	body { font-family: Arial, sans-serif; text-align: center; background-color: black; color: white; }
	#colorDisplay { width: 100px; height: 100px; margin: 20px auto; border: 1px solid #ccc; border-radius: 10px; }
	button { padding: 10px 20px; margin: 10px; font-size: 16px; cursor: pointer; }
	</style>
	<script>
	function updateColorDisplay() {
		const colorPicker = document.getElementById('colorPicker');
		const colorDisplay = document.getElementById('colorDisplay');
		const selectedColor = colorPicker.value;
		colorDisplay.style.backgroundColor = selectedColor;
		fetch(`/setRgbColor?color=${selectedColor.substring(1)}`); // Farbe an den Server senden
	}
	document.addEventListener('DOMContentLoaded', () => {
		const lastColor = "$LASTCOLOR"; // This will be replaced by the server
		const colorPicker = document.getElementById('colorPicker');
		const colorDisplay = document.getElementById('colorDisplay');
		colorPicker.value = lastColor;
		colorDisplay.style.backgroundColor = lastColor;
	});
	</script>
	</head>
	<body>
	<h1>RGB Lamp Control</h1>
	<div id='colorDisplay'>&nbsp;</div>
	<input type='color' id='colorPicker' oninput='updateColorDisplay()'>
	<p>
	<a href='/turnOff'><button>Turn Off</button></a>
	<a href='/sendPulse'><button>send pulse</button></a>
	</p>
	</body>
	</html>
	)rawliteral";

#endif
