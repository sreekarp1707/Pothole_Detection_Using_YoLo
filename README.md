AI-Based Pothole Detection and Autonomous Avoidance Car

This project combines YOLOv11-based pothole detection with an ESP32-controlled robotic car to detect potholes and perform an autonomous avoidance maneuver.

System Architecture

Camera → Python/OpenCV → YOLOv11 → Pothole Detection → Decision Logic → Wi-Fi → ESP32 → L298N Motor Driver → DC Motors

Files

- "pothole_detection.py" — Performs real-time pothole detection using YOLOv11 and sends movement commands to the ESP32.
- "esp32_car_controller.ino" — Controls the robotic car using ESP32 and L298N motor driver through Wi-Fi.
- "README.md" — Project documentation.

Technologies Used

- Python
- YOLOv11
- OpenCV
- Ultralytics
- ESP32
- L298N Motor Driver
- DC Geared Motors
- Wi-Fi Communication

Python Requirements

Install the required Python libraries:

pip install ultralytics opencv-python requests

Place the trained YOLO model file "best.pt" in the same directory as "pothole_detection.py".

Hardware Connections

ESP32 Pin| L298N Pin
GPIO 25| ENA
GPIO 26| IN1
GPIO 27| IN2
GPIO 14| IN3
GPIO 12| IN4
GPIO 33| ENB

Communication

The Python program communicates with the ESP32 through HTTP requests over Wi-Fi.

The following commands are supported:

speed:130
speed:100
speed:220
forward
left
right
stop

Speed Control

- 130 — Normal driving speed
- 100 — Reduced speed when a pothole is detected
- 220 — Turning/rotation speed

The ESP32 receives the speed command and applies the corresponding PWM value to the motor driver.

Pothole Avoidance Process

When a pothole is detected in the vehicle's path:

1. The car reduces its speed.
2. The car moves forward slowly for a short distance.
3. The car stops briefly.
4. The car rotates approximately 45° away from the pothole.
5. The car moves forward to pass the pothole.
6. The car rotates back approximately 45°.
7. The car resumes normal forward movement.

The turning angle and forward distance are controlled using calibrated time intervals.

Detection Logic

The YOLOv11 model detects potholes from the camera feed.

The center position of the detected pothole is calculated using its bounding box:

- Horizontal position determines whether the car should turn left or right.
- Vertical position determines whether the pothole is close enough to trigger the avoidance maneuver.
- Confidence threshold is set to 0.5.

Setup

1. Upload ESP32 Code

Open "esp32_car_controller.ino" in Arduino IDE and upload it to the ESP32.

The ESP32 connects to the configured Wi-Fi network and displays its assigned IP address in the Serial Monitor.

2. Configure Python

Update the "ESP32_IP" value in "pothole_detection.py" with the IP address assigned to the ESP32.

The laptop running the Python program and the ESP32 must be connected to the same Wi-Fi network or mobile hotspot.

3. Run Detection

Run:

python pothole_detection.py

The camera window will display the detected potholes and the current vehicle state.

Project Workflow

Camera
   ↓
YOLOv11 Pothole Detection
   ↓
Bounding Box Analysis
   ↓
Pothole Position Calculation
   ↓
Decision Logic
   ↓
HTTP Command
   ↓
ESP32
   ↓
L298N Motor Driver
   ↓
DC Motors
   ↓
Vehicle Avoidance

Important Notes

- The trained "best.pt" model is required for pothole detection.
- The ESP32 and laptop must be on the same network for communication.
- Motor direction can be adjusted in the ESP32 code depending on the physical motor wiring.
- Turning angle and travel distance are time-based and should be calibrated according to the vehicle.
- The system uses the laptop for real-time YOLO inference while the ESP32 handles motor control.

Project Objective

The objective is to develop an integrated AI and embedded hardware system capable of detecting potholes using computer vision and automatically controlling a robotic vehicle to avoid them.
