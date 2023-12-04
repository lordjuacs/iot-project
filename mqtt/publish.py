import random
import time
import json

from paho.mqtt import client as mqtt_client


broker = "le862a11.emqx.cloud"
port = 1883
topic = "python/mqtt"
# Generate a Client ID with the publish prefix.
client_id = f"publish-{random.randint(0, 1000)}"
username = "juacs"
password = "juacs"


def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def publish_json(client, filename):
    with open(filename) as f:
        data = json.load(f)

    for item in data:
        msg = json.dumps(item)
        result = client.publish(topic, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Sent JSON data from `{filename}` to topic `{topic}`")
        else:
            print(f"Failed to send JSON data to topic {topic}")

        # Introduce a delay of 0.5 seconds between publications
        time.sleep(5)


def run():
    client = connect_mqtt()
    client.loop_start()

    # Publish JSON data from multiple files
    for filename in ["data.json"]:
        publish_json(client, filename)

    client.loop_stop()


if __name__ == "__main__":
    run()
