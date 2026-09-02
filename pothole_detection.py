import cv2
import requests
import time
from ultralytics import YOLO

ESP32_IP         = "10.234.x.x"
MODEL_PATH       = "best.pt"
NORMAL_SPEED     = 130
SLOW_SPEED       = 100
ROTATE_SPEED     = 220
CONF_THRESHOLD   = 0.5
FRAME_LEFT       = 0.33
FRAME_RIGHT      = 0.66
ROTATE_45_TIME   = 0.3
FORWARD_PASS     = 1.8

model = YOLO(MODEL_PATH)
cap   = cv2.VideoCapture(2)

def send(cmd):
    try:
        requests.get(f"http://{ESP32_IP}/cmd?cmd={cmd}", timeout=1)
        print(f">> {cmd}")
    except Exception as e:
        print(f"Send error: {e}")

def avoid(cx_ratio):
    # 1. Slow down
    send(f"speed:{SLOW_SPEED}")
    send("forward")
    time.sleep(0.4)

    # 2. Stop for a second
    send("stop")
    time.sleep(1.0)

    # 3. Rotate ~45 degrees
    send(f"speed:{ROTATE_SPEED}")
    if cx_ratio < FRAME_LEFT:
        direction        = "right"
        return_direction = "left"
    elif cx_ratio > FRAME_RIGHT:
        direction        = "left"
        return_direction = "right"
    else:
        direction        = "right"
        return_direction = "left"

    send(direction)
    time.sleep(ROTATE_45_TIME)
    send("stop")
    time.sleep(0.3)

    # 4. Move forward ~85cm past pothole
    send(f"speed:{NORMAL_SPEED}")
    send("forward")
    time.sleep(FORWARD_PASS)
    send("stop")
    time.sleep(0.3)

    # 5. Rotate back same 45 degrees opposite direction
    send(f"speed:{ROTATE_SPEED}")
    send(return_direction)
    time.sleep(ROTATE_45_TIME)
    send("stop")
    time.sleep(0.3)

    # 6. Resume forward
    send(f"speed:{NORMAL_SPEED}")
    send("forward")

# Start
send(f"speed:{NORMAL_SPEED}")
send("forward")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Camera error")
        break

    frame_h = frame.shape[0]
    frame_w = frame.shape[1]
    results  = model(frame, conf=CONF_THRESHOLD)[0]
    detected = False

    for box in results.boxes:
        x1, y1, x2, y2 = map(int, box.xyxy[0])
        cy       = (y1 + y2) / 2
        cx       = (x1 + x2) / 2
        cx_ratio = cx / frame_w
        cy_ratio = cy / frame_h

        label = f"{results.names[int(box.cls[0])]} {box.conf[0]:.2f}"
        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)
        cv2.putText(frame, label, (x1, y1 - 8),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)

        if cy_ratio > 0.5:
            detected = True
            cv2.putText(frame, "AVOIDING", (10, 40),
                        cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 0, 255), 3)
            cv2.imshow("YOLOv11 - Pothole Detection", frame)
            avoid(cx_ratio)
            break

    if not detected:
        cv2.putText(frame, "CRUISING", (10, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 255, 0), 3)
        send(f"speed:{NORMAL_SPEED}")
        send("forward")

    cv2.imshow("YOLOv11 - Pothole Detection", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

send("stop")
cap.release()
cv2.destroyAllWindows()
