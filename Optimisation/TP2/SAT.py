def certificat(a, b, c, d):
    return (a or b) and (a or not c) and (not d or a) and (not c or not d)

def certificat2(a, b, c, d):
    print(a, b, c, d)
    return a and (not b) and (not c) and (not d)

saisie = "0000"

while(saisie):
    saisie = input("\n> ")
    if len(saisie) < 4:
        continue
    
    if (certificat(*map(int, saisie))):
        print("Congratulations!")
        break