import random
import hashlib
import os

def Miller_Rabin_primality_test(n, k = 50): #50 iterations to make the probability of error negligible
    if n == 2 or n == 3: return True
    if n % 2 == 0 or n < 2: return False

    s, t = 0, n - 1

    while t % 2 == 0: #n - 1 = 2^s * t, where t is odd 
        s += 1
        t //= 2

    for _ in range(k):
        a = random.randrange(2, n - 1)
        x = pow(a, t, n)

        if x == 1 or x == n - 1:
            continue

        for _ in range(s - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
        
    return True

def generate_parameters(q_bits, p_bits):
    while True:
        q = random.getrandbits(q_bits) #we first find q and calculate p = kq + 1

        if q % 2 != 0 and Miller_Rabin_primality_test(q):
            break

    min_p_value = pow(2, p_bits - 1)
    k = min_p_value // q
    if k % 2 != 0: k += 1 #for p to be prime k must be even, since q is odd and we add 1 to it
        
    while True:
        p = k * q + 1
        if Miller_Rabin_primality_test(p) and p.bit_length() >= p_bits:
            break
        k += 2 #since we ensured that k is even and must remain so, we add 2 for acceleration

    while True:
        h = random.randint(2, p - 2)
        exponent = (p - 1) // q
        g = pow(h, exponent, p) #we calculate the generator g of order q as a random element h of Z*p, raised to the power (p-1)/q. For this g^q = 1 holds, but we must ensure g is not raised to 1, since q is prime

        if g > 1: #since g > 1 (g ^ 1 = 1 mod p), the order of g will be q
            break
            
    return p, q, g

def calculate_challenge(g, vk, T, filename, q):
    hash_object = hashlib.sha256() #we use the sha-256 hash function
    hash_object.update(f"{g}|{vk}|{T}|".encode()) #we convert integers to bytes
    
    with open(filename, "rb") as f:
        while chunk := f.read(4096):
            hash_object.update(chunk) #since we want large files, we split the file into 4KB chunks for more efficient management

    hash_bytes = hash_object.digest() 
    
    c_uncut = int.from_bytes(hash_bytes, byteorder='big') #we convert bytes to integer
    
    c = c_uncut % q #we want the challenge c to belong to Z*q
    
    return c

def verify_signature(p, q, g, vk, filename, signature):
    c, s = signature

    T_calculated = (pow(g, s, p) * pow(vk, q - c, p)) % p #vk^(-c) (mod p) = vk^(q-c) (mod p)
    c_calculated = calculate_challenge(g, vk, T_calculated, filename, q) #check if c = H(g, vk, g^s * vk^(-c), m)

    return c == c_calculated

def main():
    filename = "ΖΠΓ.pdf" #here we input the path of the file we want to sign

    if not os.path.exists(filename):
        with open(filename, "wb") as f:
            f.write(b"Message to be signed by Schnorr signature scheme.")

    print("Generating parameters. This may take a while...")

    q_bits = 256
    p_bits = 2048
    p, q, g = generate_parameters(q_bits, p_bits)

    print(f"p = {p}")
    print(f"q = {q}")
    print(f"g = {g}")

    sk = random.randint(0, q - 1)
    vk = pow(g, sk, p)

    print(f"vk = {vk}")

    t = random.randint(0, q - 1)
    T = pow(g, t, p)

    print(f"T = {T}")

    c = calculate_challenge(g, vk, T, filename, q)

    s = (t + c * sk) % q

    signature = (c, s)

    print(f"σ = (c, s) = {signature}")

    check = verify_signature(p, q, g, vk, filename, signature)

    if check:
        print("Signature is verified!")
    else:
        print("Signature is not accepted!")

if __name__ == "__main__":
    main()