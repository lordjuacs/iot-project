import random
from paho.mqtt import client as mqtt_client
import psycopg2
import time
import json

broker = "le862a11.emqx.cloud"
port = 1883
topic = "python/mqtt"
client_id = f"subscribe-{random.randint(0, 100)}"
username = "juacs"
password = "juacs"

# PostgreSQL connection parameters
dbname = "iot"
user = "postgres"
password_db = "314159"
host = "localhost"
port_db = "5432"

table_name = "air_quality"
insert_data_query = f"INSERT INTO {table_name} (id, ppmCO, ppmCO2, ppmAlcohol, ppmAcetona, ppmNH4, ppmTolueno, time) VALUES (%s, %s, %s, %s, %s, %s, %s, %s)"


def connect_mqtt() -> mqtt_client:
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


def insert_into_postgres(conn, data):
    try:
        cursor = conn.cursor()

        # Get the current timestamp
        current_timestamp = time.strftime("%Y-%m-%d %H:%M:%S")

        # Execute the INSERT query
        cursor.execute(
            insert_data_query,
            (
                data["id"],
                data["ppmCO"],
                data["ppmCO2"],
                data["ppmAlcohol"],
                data["ppmAcetona"],
                data["ppmNH4"],
                data["ppmTolueno"],
                current_timestamp,
            ),
        )
        conn.commit()

        print("Inserted data into PostgreSQL table!")
    except Exception as e:
        print(f"Failed to insert data into PostgreSQL table: {e}")
        conn.rollback()
    finally:
        cursor.close()


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

        # Assuming your MQTT payload is in JSON format
        try:
            data = json.loads(msg.payload.decode())
            insert_into_postgres(postgres_conn, data)
        except Exception as e:
            print(f"Error processing MQTT payload: {e}")

    client.subscribe(topic)
    client.on_message = on_message


# Initialize PostgreSQL connection
postgres_conn = psycopg2.connect(
    dbname=dbname, user=user, password=password_db, host=host, port=port_db
)


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


if __name__ == "__main__":
    run()
