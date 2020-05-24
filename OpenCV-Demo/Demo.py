import paho.mqtt.client as mqtt 
import cv2 
import numpy as np

buff=[]
imgready = False

def on_message(client, userdata, message):
    global buff
    global imgready
    print("message received ")
    imgready = True
    buff = message.payload

broker_address="BROKER IP" #Replace with your broker adress
client = mqtt.Client("Test")
client.on_message=on_message

client.username_pw_set("USER", "PASSWORD") #Replace with your User/Pass

client.connect(broker_address) 

client.subscribe("MQTT TOPIC") #Replace with your Topic

while(True): 
    client.loop_start()
    if(imgready == True):
        nparr = np.frombuffer(buff, np.uint8)
        img = cv2.imdecode(nparr,1)
        cv2.imshow('Demo', img) 
    if cv2.waitKey(1) & 0xFF == ord('q'): 
        break