import psycopg2

# PostgreSQL connection parameters for your local database
db_host = "localhost"
db_port = 5432
db_name = "iot"
db_user = "postgres"
db_password = "314159"

# PostgreSQL table name
table_name = "air_quality"

# SQL query to create the table
create_table_query = """
CREATE TABLE IF NOT EXISTS {} (
    id VARCHAR(255),
    ppmCO DOUBLE PRECISION,
    ppmCO2 DOUBLE PRECISION,
    ppmAlcohol DOUBLE PRECISION,
    ppmAcetona DOUBLE PRECISION,
    ppmNH4 DOUBLE PRECISION,
    ppmTolueno DOUBLE PRECISION,
    time TIMESTAMP
);
""".format(
    table_name
)


def create_table():
    try:
        conn = psycopg2.connect(
            host=db_host,
            port=db_port,
            database=db_name,
            user=db_user,
            password=db_password,
        )
        cursor = conn.cursor()

        # Execute the CREATE TABLE query
        cursor.execute(create_table_query)
        conn.commit()

        print(f"Table '{table_name}' created successfully.")
    except Exception as e:
        print(f"Failed to create table: {e}")
    finally:
        if conn:
            conn.close()


if __name__ == "__main__":
    create_table()
