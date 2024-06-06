def initialize_names_cards(names_cards):
    with open('humansfile/names_cards.txt', 'r') as file:
        for line in file:
            mykey=line.split('=')[0].strip().replace(' ','').lower()
            names_cards[mykey] =  line.split('=')[1].strip()
    

def initialize_doctors_cards(doctors_cards):
    with open('humansfile/doctors.txt', 'r') as file:
        for line in file:
            doctors_cards.append(line.strip().replace(' ', '').lower())

def initialize_nurse_cards(nurses_cards):
    with open('humansfile/nurses.txt', 'r') as file:
        for line in file:
            nurses_cards.append(line.strip().replace(' ','').lower())

def initialize_patients_cards(patients_cards):
    with open('humansfile/patients.txt', 'r') as file:
        for line in file:
            patients_cards.append(line.strip().replace(' ','').lower())

def initialize_devices_cards(devices_cards):
    with open('humansfile/devices.txt','r') as file:
       for line in file:
           devices_cards.append(line.strip().replace(' ','').lower())
