import psycopg2

conn = psycopg2.connect(database="simplesalesprocessor")
print("Connection created")

conn.close()