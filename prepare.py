import nacl.utils
import sqlite3
import random

from nacl.public import PrivateKey, Box


names = ["Rincewind", "DEATH", "Sam Vimes", "Nanny Ogg", "Vetinari", "Angua", "Twoflower", "Carrot", "Fred Colon", "Nobby" ]
def c_array(b):
    return ', '.join([ str(hex(b[x])) for x in range(len(b)) ])
# Generate Bob's private key, which must be kept secret
skbob = PrivateKey.generate()

# Bob's public key can be given to anyone wishing to send
#   Bob an encrypted message
pkbob = skbob.public_key

# Alice does the same and then Alice and Bob exchange public keys
skalice = PrivateKey.generate()
pkalice = skalice.public_key

# Bob wishes to send Alice an encrypted message so Bob must make a Box with
#   his private key and Alice's public key
bob_box = Box(skbob, pkalice)

alice_box = Box(skalice, pkbob)

con = sqlite3.connect("tutorial.db")
cur = con.cursor();
cur.execute("CREATE TABLE taxes(name TEXT, ciphertext BLOB, nonce BLOB);")
for name in names:
    income = random.randint(1, 10000)
    print(name, income)
    encrypted = bob_box.encrypt(income.to_bytes(8, byteorder='little'))
    cur.execute("INSERT INTO taxes VALUES(?, ?, ?)", (name, encrypted.ciphertext, encrypted.nonce))
    plaintext = alice_box.decrypt(encrypted)
    print(int.from_bytes(plaintext, byteorder='little'))

con.commit()

print(c_array(bytes(skbob.public_key)))
print("---")
print(c_array(bytes(skalice)))
print("---")

