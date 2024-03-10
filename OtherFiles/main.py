import serial
import time
import firebase_admin
from firebase_admin import credentials, messaging

cred_path = "C:/Users/sros1/Downloads/grizzhacks-416720-firebase-adminsdk-8gb8t-6a91112448.json"
cred = credentials.Certificate(cred_path)
firebase_admin.initialize_app(cred)

ser = serial.Serial('COM5', 9600)

def send_notification(device_token):
    message = messaging.Message(
        notification=messaging.Notification(
            title='Pothole detected! Confirm pothole?',
            body='Yes or No',
        ),
        token=device_token,
    )
    response = messaging.send(message)
    print('Successfully sent message:', response)

def check_ser_and_notify():
        prev = '0'
        data = '1'
        while True:
            if ser.inWaiting() > 0:
                data = ser.readline().decode('utf-8').rstrip()
                print(data)
                if "1" in data:
                    if "0" in prev:
                        print('Vibration detected')
                        send_notification("eux83WT6TXeIQUw4yKF1Qp:APA91bFCT8ag6RNkDVU0npVwszsMzkw406EsM-SHDC8rzKEBOvSZjO1mRqmb3fjaZyd1EF3OnQAI3C9fqUbW2ynjIUnKhSyerKVHsHSzuSELFMcTnxvxOKOhXrQQdGnZu9A5ZVDQNwgD")
                        time.sleep(1)

                prev = data


# Run the check
check_ser_and_notify()
