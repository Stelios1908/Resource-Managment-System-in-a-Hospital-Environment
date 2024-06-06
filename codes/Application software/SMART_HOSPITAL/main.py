import pygame
import pygame_widgets
from pygame_widgets.combobox import ComboBox
from pygame_widgets.button import Button
from pygame_widgets.dropdown import Dropdown
import sys
from pygame.locals import *
import serial
import subprocess
import shutil
import time
from datetime import datetime, timedelta
from buttons import buttons,surfaces_for_buttons
from rooms_design_positions import rooms,corridors
from indications_positions import temperatures,humidity,connection_pos,connection_pos_new
from resources_positons import rooms_pos_for_nurse,rooms_pos_for_doctors,rooms_pos_for_patients,rooms_pos_for_devices,doctors_list,nurses_list,patients_list,devices_list,names_cards
import functions
from datetime import datetime

from functions import readCards,details
from initialize_humans_cards import initialize_names_cards,initialize_doctors_cards, initialize_nurse_cards, initialize_patients_cards,initialize_devices_cards



# colors
white = (255, 255, 255)
red  = (255, 0, 0)
blue = (0, 0, 255)
gray = (200, 200, 200)
black = (0, 0, 0)
green = (0, 255, 0)
button_color = (100, 100, 100)

color_security = red
color_patient = red

id_for_lock=[]
name_for_lock=[]
prev_name_for_lock=[]
again_alarm_lock=False

room_card_list = {
    "room1": {},
    "room2": {},
    "room3": {},
    "room4": {},
    "room5": {},
    "room6": {}
}

#Αρχικοποίηση των πινάκων
initialize_names_cards(names_cards)
initialize_doctors_cards(doctors_list)
initialize_nurse_cards(nurses_list)
initialize_patients_cards(patients_list)
initialize_devices_cards(devices_list)



#Ανοιγμα σειριακή θύρα USB που είναι συνδεδεμένο το NodeMCU
try:
   ser = serial.Serial('COM6', 115200, timeout=1)
   print("serial OK")
   serIsOk = True
except serial.SerialException as e:
    # Εκτύπωση μηνύματος σφάλματος και συνέχιση του προγράμματος
    print(f"Σφάλμα: {e}")
    print("Δεν anoixe")
    serIsOk = False


# Αρχικοποίηση Pygame
pygame.init()

# Παράθυρο 1300 800
window_size = (1510, 800)
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption('SMART HOSPITAL')

channel=0
first_time_fire=True

# Για search
search_mode = False
first_search=False
first_time_call=False
selected_item=""


#sinartiseis gia pati9ma enos koumpiou
def output():
    global search_mode
    global selected_item
    global first_search
    selected_item = comboBox.getText()
    search_mode=True
    first_search=True

def change_mytext():
    global selected_for_comb
    selected_for_comb = comboBox_Lock.getText()

    comboBox_Lock.textBar.colour=(204, 255, 255)
    comboBox_Lock.textBar.textColour=(0, 0, 0)

    for key,val in names_cards.items():
        if val == selected_for_comb:
            if key in   id_for_lock:
              comboBox_Lock.textBar.colour=(0, 127, 10)
              comboBox_Lock.textBar.textColour=(255, 255, 255)
            
def lock():
    global selected_for_lock
    global color_patient
    global again_alarm_lock

    
    selected_for_lock= comboBox_Lock.getText()
    
    for key,val in names_cards.items():
        if val == selected_for_lock:
            if key not in  id_for_lock:
               id_for_lock.append(key)
               change_mytext()
               print("Selected for lock : ", id_for_lock)

    if  not id_for_lock:
        color_patient = (255, 0, 0)
    else:
   	    color_patient = (0, 250, 0)


def unlock():
    global selected_for_unlock
    global color_patient
    selected_for_unlock= comboBox_Lock.getText()
    
    for key,val in names_cards.items():
        if val == selected_for_unlock:
            if key in id_for_lock:
               id_for_lock.remove(key)
               change_mytext()
               print("Selected for lock : ", id_for_lock)
    
    
    if  not id_for_lock:
        color_patient = (255, 0, 0)
    else:
   	     color_patient = (0, 250, 0)
    

def myUnlocks(list):
    global color_patient
    
    print("2222222")
    for elem in list:
        for key,val in names_cards.items():
            if val == elem:
                print("KET IS : ", key)
                print(id_for_lock)
                if key in id_for_lock:
                   id_for_lock.remove(key)
                   change_mytext()
                   print("Selected for lock : ", id_for_lock)
    
    
    if  not id_for_lock:
        color_patient = (255, 0, 0)
    else:
   	     color_patient = (0, 250, 0)
    

# gia tin anazitisi poroy se domatio
myChoise = list(names_cards.values())
comboBox = ComboBox(
    screen, 1250, 25, 200, 40, name='Select Color',
    choices=myChoise,
    maxResults=1,
    font=pygame.font.SysFont('calibri', 20),
    borderRadius=10,  colour=(204, 255, 255), direction='down',
    textHAlign='left'
)

Search_button = Button(
    screen, 1180, 25, 70, 40, text='Serch', fontSize=10,
    margin=10, inactiveColour=(0, 204, 204), pressedColour=(0, 255, 0),
    radius=0, onClick=output, font=pygame.font.SysFont('calibri', 20),
    borderColour=(0, 0, 0) ,
    textVAlign='bottom'
)

#gia tin apagoreysi na figi apo domatio
myChoise_Lock = [
    {"text": "MINA GOYMA", "color": (255, 0, 0)},  # Red color
    {"text": "JOHN MISIRLIS", "color": (0, 255, 0)}  # Green color
]
comboBox_Lock = ComboBox(
    screen, 1250, 590, 150, 40, name='Select Color',
    choices=[choice["text"] for choice in myChoise_Lock],
    maxResults=1,
    font=pygame.font.SysFont('calibri', 20),
    borderRadius=10,  colour=(204, 255, 255), direction='down',
    textHAlign='left'
)

Lock_button = Button(
    screen, 1180, 590, 70, 40, text='LOCK', fontSize=10,textColour=(255, 255, 255),
    margin=10, inactiveColour=(0, 127, 0), pressedColour=(0, 255, 0),
    radius=10, onClick=lock, font=pygame.font.SysFont('calibri', 20),
    borderColour=(0, 0, 0) ,
    textVAlign='bottom'
)

Unlock_button = Button(
    screen, 1400, 590, 70, 40, text='UNLOCK', fontSize=10, textColour=(255, 255, 255),
    margin=10, inactiveColour=(127, 0, 0), pressedColour=(0, 255, 0),
    radius=10, onClick=unlock, font=pygame.font.SysFont('calibri', 20),
    borderColour=(0, 0, 0) ,
    textVAlign='bottom'
)


background = pygame.image.load('foto/background1.png')
desired_size = (1510, 800) 
background = pygame.transform.scale(background, desired_size)
screen.blit(background, (0, 0))
#pygame.display.update()




#μεταβλητές για να εμφανίζοντε τα componet
temperature_visible = False
humidity_visible =False
nurses_visible =  False
doctors_visible = False
patients_visible =False
devices_visible = False
details_visible = False
all_visible = False
clear_alarm_visible = False
fire_visible = True
log_file_visible = False
clear_search_visible = False
mute_alarm_visible = False
yes_visible = False   
no_visible = False

# Μεταβλητ για λαθος αναγνωση που βοηθαει στην εμφανιση wifi λυχνια
fault_message = True

# Κείμενο που θα εμφανίζεται στα κουμπια
message_font = pygame.font.Font(None, 24)
temp_hum_font = pygame.font.Font(None, 30)

message = ""

# Κείμενο στα κουμπιά
font = pygame.font.Font(None, 36)

for temp in temperatures:
    temp['value'] = 21.4 # Τυχαίες τιμές για το παράδειγμα

for humi in humidity:
    humi['value'] = 50  # Τυχαίες τιμές για το παράδειγμα

# Φορτώνουμε υχο σειρήνας  και θυρών 
sound_file = "sound/alarm.mp3" 
sound = pygame.mixer.Sound(sound_file)

sound_file_doors = "sound/doors.wav" 
sound_doors = pygame.mixer.Sound(sound_file_doors)

sound_file_alert = "sound/alert.wav"
sound_alert = pygame.mixer.Sound(sound_file_alert)


# Φορτώνουμε τις εικόνες 
nurse_image = pygame.image.load('foto/nurse.png')
doctor_image=  pygame.image.load('foto/doctor.png')
patient_image = pygame.image.load('foto/patient.png')
device_image = pygame.image.load('foto/device.png')
room_image = pygame.image.load('foto/room_floor.png')  # Αντικαταστήστε το 'background_image.jpg' με τη δική σας εικόνα
wifi_image = pygame.image.load('foto/wifi1.png')
battery_image = pygame.image.load('foto/battery.png')
fire_image = pygame.image.load('foto/fire.png')

# Προσαρμόζουμε τις διαστάσεις της εικόνας
new_size_nurse = (nurse_image.get_width() // 8, nurse_image.get_height() // 8)
new_size_doctor = (nurse_image.get_width() // 10, doctor_image.get_height() // 10)
new_size_patient = (patient_image.get_width() // 10, patient_image.get_height() // 10)
new_size_device  = (device_image.get_width() // 10, device_image.get_height() // 10)
new_size_wifi = (wifi_image.get_width() // 10, wifi_image.get_height() // 10)
new_size_battery = (battery_image.get_width() // 2, battery_image.get_height() // 2)
new_size_fire = (fire_image.get_width() // 5, fire_image.get_height() // 5)

resized_image_nurse = pygame.transform.scale(nurse_image, new_size_nurse)
resized_image_doctor = pygame.transform.scale(doctor_image, new_size_doctor)
resized_image_patient = pygame.transform.scale(patient_image, new_size_patient)
resized_image_device =pygame.transform.scale(device_image,new_size_device)
resized_image_rooms = pygame.transform.scale(room_image, window_size)
resized_image_wifi = pygame.transform.scale(wifi_image, new_size_wifi)
resized_image_battery = pygame.transform.scale(battery_image, new_size_battery)
resized_image_fire = pygame.transform.scale(fire_image, new_size_fire)
resized_image_fire.set_alpha(170)
# Συντεταγμένες για τη θέση της μικρογραφίας
position = (window_size[0] // 2 - new_size_nurse[0] // 2, window_size[1] // 2 - new_size_nurse[1] // 2)

start_time = datetime.now()
start_time_sound = datetime.now()
interval_seconds = 0.50

start_time_lock = datetime.now()


time_with_no_check=25
last_timer_for_check_connections = datetime.now()
connection_ok_for_print=[False, False, False, False, False, False, False]
connection_ok  = [False, False, False, False, False, False, False]

current_selection=""
color =1
# Κύριος βρόχος
while True:

     # Ενημέρωση της τρέχουσας επιλογής του ComboBox GIA TO LOCK
    if comboBox_Lock.getText() != current_selection:
        current_selection = comboBox_Lock.getText()
        change_mytext()

    # Υπολογισμός διαφοράς χρόνου από την αρχική στιγμή
    elapsed_time = datetime.now() - start_time
    elapsed_time_sound = datetime.now() - start_time_sound
    elapsed_time_lock = datetime.now() - start_time_lock

    #for alarm
    if elapsed_time >= timedelta(seconds=interval_seconds):
        color =1 + color

        #σχεδιαση alarm στις εισοδοθς των δωματιων και διαδρόμου
        for i in range(8,14):
            
            if (corridors[i]['alarm']=='1' or corridors[i]['lockAlarm']=='1'):
                corridors[i]['lockAlarm']='1'
                if color % 2==1:
                    corridors[i]['color']=red
                    if not mute_alarm_visible:
                       sound_doors.play(fade_ms=0)
                else:             
                   corridors[i]['color']=gray
                   sound_doors.stop()


        #σχεδιαση alarm για τα παραθυρα
        for i in range(14,20):
            
            if (corridors[i]['alarm']=='1' or corridors[i]['lockAlarm']=='1'):
                corridors[i]['lockAlarm']='1'
                if color % 2==1:
                    if not mute_alarm_visible:
                       corridors[i]['color']=red
                       sound_doors.play(fade_ms=0)
                else:       
                   
                   corridors[i]['color']=gray
                   sound_doors.stop()
                   sound_doors.stop()
            else: 
                corridors[i]['color']=black

        start_time=datetime.now()
           
    #ποιανουμε event
    events = pygame.event.get()
    for event in events:
        if event.type == pygame.QUIT:
            ser.close()
            pygame.quit()
            sys.exit()
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button==1:
            for button in buttons:
               if button['rect'].collidepoint(event.pos):
                    
                    if button['text']=='TEMPERATURE':
                       temperature_visible = not temperature_visible
                       details_visible=False
                      
                    if button['text']=='HUMIDITY':
                       humidity_visible = not humidity_visible
                       details_visible=False
                       
                    if button['text']=='NURSES':
                       nurses_visible = not nurses_visible   
                       details_visible=False
                       
                    if button['text']=='DOCTORS':
                       doctors_visible = not doctors_visible
                       details_visible=False
                       
                    if button['text']=='DEVICES' :
                       devices_visible = not devices_visible   
                       details_visible=False
                       
                    if button['text']=='PATIENTS':
                       patients_visible = not patients_visible  
                       details_visible=False
                       
                    if button['text']=='DETAILS' :
                       details_visible= not details_visible

                    if button['text']=='YES' :
                       yes_visible= True
                       no_visible=False
                       send=True

                    if button['text']=='NO' :
                       no_visible= True
                       yes_visible=False
                       send=True
        
                    if button['text']=='CLEAR_ALARM':
                        for i in range(8,20):
                           corridors[i]['lockAlarm']='0'   
                           corridors[i]['alarm']='0'
                           if i<14:
                              corridors[i]['color']=gray
                           else:
                               corridors[i]['color']=black
                           first_time_fire=True
                           sound.stop()
                           sound_doors.stop()
                           sound_alert.stop()
                           mute_alarm_visible=False

                           
                        myUnlocks(name_for_lock)
                        name_for_lock=[]
                              
                         


                        for elem in rooms:
                            elem['lockFire']='0'
                        clear_alarm_visible=False        
                               
                    if button['text']=='ALL':
                       all_visible=True
                       temperature_visible = True
                       humidity_visible = True  
                       nurses_visible=True
                       doctors_visible=True
                       patients_visible=True
                       devices_visible=True
                       batteries_visible=False
                       details_visible=False

                    if button['text']=='CLEAR_SEARCH':
                        if search_mode:
                            search_mode=False
                      
                    
                    if button['text']=='MUTE_ALARM':
                       mute_alarm_visible = not mute_alarm_visible
                    
                    if button['text']=='CLEAR':
                       temperature_visible = False
                       humidity_visible = False
                       nurses_visible=False
                       doctors_visible=False
                       patients_visible=False
                       devices_visible=False
                       details_visible=False
                       all_visible=False
                       batteries_visible=False
                      
                       

                    if button['text']=='LOG_FILE':
                       
                       # The path to the file you want to copy
                       file_path = "C:\\Users\\30698\\Desktop\\sideris\\PTIXIAKI\\CODE\SMART_HOSPITAL\\log_temp.xlsx"

                       # The path to the destination (same folder in this case)
                       destination_path = "C:\\Users\\30698\Desktop\\sideris\\PTIXIAKI\\CODE\\SMART_HOSPITAL\\log.xlsx"

                       shutil.copy2(file_path, destination_path)
                       program_path = "C:\\Users\\30698\\Desktop\\Read_excel\\Read_excel\\bin\\Debug\\net6.0-windows\\Read_excel.exe"
                      
                       # Use subprocess.run to execute the program
                       #subprocess.run([program_path], shell=True)

                       subprocess.Popen([program_path])
              
    
    #καθαρισμοs και εισαγωγη φωτο φοντου
    screen.fill(white)
    screen.blit(background, (0, 0))

    # Σχεδίαση των δωματίων
    for room in rooms:
        room_surface = pygame.Surface(room['size'])    
        room_surface.blit(resized_image_rooms, (0, 0))
        pygame.draw.rect(screen, gray, (*room['position'], *room['size']))
        screen.blit(room_surface, room['position'])
        pygame.draw.rect(screen, black, (*room['position'], *room['size']), 3)
       
    # Σχεδίαση των διαδρομών
    for corridor in corridors:
        pygame.draw.line(screen,corridor['color'], corridor['start'], corridor['end'], corridor['size'])
              
    # Σχεδίαση των επιφανειων για τα μπουτον 
    for iter,surface_info in enumerate(surfaces_for_buttons,start=0):
        if iter == 5 or iter==6:
            rad = 10
            rect = pygame.Rect(surface_info["position"], surface_info["size"])
            
            # Σχεδίαση του εσωτερικού διαφανούς κουμπιού
            inner_surface = pygame.Surface(surface_info["size"], pygame.SRCALPHA)
            pygame.draw.rect(inner_surface, (255, 255, 255, 0), inner_surface.get_rect(), border_radius=rad)
            screen.blit(inner_surface, surface_info["position"])
            
            # Σχεδίαση του μαύρου περιγράμματος
            pygame.draw.rect(screen, black, rect, width=2, border_radius=rad)
        else:
            rad=10
            rect = pygame.Rect(surface_info["position"], surface_info["size"])
            if iter == 0 or iter == 3 or iter == 4:
                pygame.draw.rect(screen, black, rect, border_radius=10)
            pygame.draw.rect(screen, surface_info["color"], rect.inflate(-6, -6), border_radius=10)
        
    for button in buttons:
        current_button_on = str(button['text']).lower() + "_visible"
        if  current_button_on!='clear_visible' and globals()[current_button_on]:
            rad = 8
        else:
            rad= 2
        pygame.draw.rect(screen, button['color'], button['rect'],border_radius=10)
        pygame.draw.rect(screen, black, button['rect'], rad,border_radius=10)


        # Εμφάνιση του κειμένου στα κουμπιά 
        button['text_surface'] = message_font.render(button['text'], True, button['text_color'])
        if button['text_rect'] is None:
              button['text_rect'] = button['text_surface'].get_rect(center=button['rect'].center)
        screen.blit(button['text_surface'], button['text_rect'])   
             
   
    #Εμφάνιση θερμοκρασια
    if temperature_visible and not search_mode:
        for temp in temperatures:
            pygame.draw.rect(screen, red, (*temp['position'], *temp['size']))
            pygame.draw.rect(screen, black, (*temp['position'], *temp['size']), 4)
            temperature_value = temp['value']
          
            # Δημιουργία επιφάνειας με το κείμενο
            text_surface = temp_hum_font.render(f"{temperature_value}°C", True, white)

             # Εμφάνιση του κειμένου στην οθόνη στο κέντρο του ορθογωνίου
            text_rect = text_surface.get_rect(center=(temp['position'][0] + temp['size'][0] // 2, temp['position'][1] + temp['size'][1] // 2))
            screen.blit(text_surface, text_rect)
            
    #Εμφάνιση υγρασιας
    if humidity_visible and not search_mode:
        for humi in humidity:
            pygame.draw.rect(screen, blue, (*humi['position'], *humi['size']))
            pygame.draw.rect(screen, black, (*humi['position'], *humi['size']), 4)

            humidity_value = humi['value']

            # Δημιουργία επιφάνειας με το κείμενο
            text_surface = temp_hum_font.render(f"{humidity_value}%", True, white)
            
            # Εμφάνιση του κειμένου στην οθόνη στο κέντρο του ορθογωνίου
            text_rect = text_surface.get_rect(center=(humi['position'][0] + humi['size'][0] // 2, humi['position'][1] + humi['size'][1] // 2))
            screen.blit(text_surface, text_rect)

    #Εμφάνιση νοσοκομες
    if nurses_visible and not search_mode :
        room6 = list({key for key, value in room_card_list["room6"].items() if value == "nurse"})
        for iter,row  in enumerate(rooms_pos_for_nurse,start=1):
            if iter != 6:
                current_room_nurse = list({key for key, value in room_card_list["room"+str(iter)].items() if value == "nurse"})            
                for iter,element in enumerate(row,start=0):
                    if iter >= len(current_room_nurse):
                          break
                    position=element['position']
                    screen.blit(resized_image_nurse,position)
                    if current_room_nurse[iter] in room6:
                        room6.remove(current_room_nurse[iter])
            else:
                for iter,element in enumerate(row,start=0):
                    if iter >= len(room6):
                          break
                    position=element['position']
                    screen.blit(resized_image_nurse,position)

    #Εμφάνιση γιατροι
    if doctors_visible and not search_mode:
        room6 = list({key for key, value in room_card_list["room6"].items() if value == "doctor"})
        for iter,row  in enumerate(rooms_pos_for_doctors,start=1):

            if iter != 6:
                current_room_doctor =list( {key for key, value in room_card_list["room"+str(iter)].items() if value == "doctor"})
                # print("room"+str(iter) + " : " + str(current_room_doctor))
                
                for iter,element in enumerate(row,start=0):
                    if iter >= len(current_room_doctor):
                          break
                    position=element['position']
                    screen.blit(resized_image_doctor,position)
                    if current_room_doctor[iter] in room6:
                        room6.remove(current_room_doctor[iter])
            else:
                for iter,element in enumerate(row,start=0):
                    if iter >= len(room6):
                          break
                    position=element['position']
                    screen.blit(resized_image_doctor,position)
    
    #Εμφάνιση ασθενείς
    if patients_visible and not search_mode:
        room6 = list({key for key, value in room_card_list["room6"].items() if value == "patient"})
        for iter, row  in enumerate(rooms_pos_for_patients,start=1):
            if iter != 6:
                current_room_patient = list({key for key, value in room_card_list["room"+str(iter)].items() if value == "patient"})
                for iter,element in enumerate(row,start=0):
                    if iter >= len(current_room_patient):
                          break
                    position=element['position']
                    screen.blit(resized_image_patient,position)
                    if current_room_patient[iter] in room6:
                        room6.remove(current_room_patient[iter])
            else:
                for iter,element in enumerate(row,start=0):
                    if iter >= len(room6):
                          break
                    position=element['position']
                    screen.blit(resized_image_patient,position)
                   
    #Εμφάνιση συσκευες
    if devices_visible and not search_mode:
        room6 = list({key for key, value in room_card_list["room6"].items() if value == "device"})
        for iter,row in enumerate(rooms_pos_for_devices,start=1):
            if iter != 6:
                current_room_device =list( {key for key, value in room_card_list["room"+str(iter)].items() if value == "device"})
                for iter,element in enumerate(row,start=0):
                    if iter >= len(current_room_device):
                       break
                   
                    position=element['position']
                    screen.blit(resized_image_device,position)
                    if current_room_device[iter] in room6:
                        room6.remove(current_room_device[iter])

                    '''
                    if	details_visible:
                        text = current_room_device[iter]
                        font_id_devices = pygame.font.Font(None, 20)
                        text_surface = font_id_devices.render(text, True, (93, 0, 0)) 
                        text_rect = text_surface.get_rect(center=(position[0] + 25, position[1] + 25))
                        screen.blit(text_surface, text_rect)
                    '''
            else:
                for iter,element in enumerate(row,start=0):
                    if iter >= len(room6):
                          break
                    position=element['position']
                    screen.blit(resized_image_device,position)               
 
    #εμφανιση search μηνηματος
    if search_mode:
        id_search=None
        room_number=None
        for key,val in names_cards.items():
            if val==selected_item:
                id_search=key
                break
        if id_search:
            for indx in range(0,6):
                if id_search in room_card_list["room"+str(indx+1)]:
                    room_number=indx+1
                    break   



        if room_number:
            x =  rooms[room_number-1]['position'][0]
            y =  rooms[room_number-1]['position'][1]
            W =  rooms[room_number-1]['size'][0]
            H =  rooms[room_number-1]['size'][1]
            
            final_x = x+W//2
            final_y = y+H//2 
            
            text = "HERE IS "+ selected_item
            # Δημιουργία επιφάνειας εικόνας που περιέχει το κείμενο
            text_surface = font.render(text, True, red)
            # Θέση του κειμένου στο παράθυρο
            text_rect = text_surface.get_rect()
            text_rect.center = (final_x,final_y)
            # Εμφάνιση της επιφάνειας εικόνας που περιέχει το κείμενο
            screen.blit(text_surface, text_rect)
            first_search=False
        else:

            x =  rooms[-1]['position'][0]
            y =  rooms[-1]['position'][1]
            W =  rooms[-1]['size'][0]
            H =  rooms[-1]['size'][1]
            final_x = x+W//2
            final_y = y+H//2 

            text = "NOT FOUND"
        
            # Δημιουργία επιφάνειας εικόνας που περιέχει το κείμενο
            text_surface = font.render(text, True, red)
            # Θέση του κειμένου στο παράθυρο
            text_rect = text_surface.get_rect()
            text_rect.center = (final_x,final_y)
            # Εμφάνιση της επιφάνειας εικόνας που περιέχει το κείμενο
            screen.blit(text_surface, text_rect)
            first_search=False

        
   
    #Σχεδίαση φωτιάς και υχιτικο    
    for element in rooms:
        current_time = pygame.time.get_ticks()
        sound_length = 6
        
        if element['fire']=='1' or element['lockFire']=='1':

            if first_time_fire or elapsed_time_sound >= timedelta(seconds=6):
                humidity_visible = False
                first_time_fire=False
                temperature_visible = True
                nurses_visible =  True
                doctors_visible = True
                patients_visible =True

                if not mute_alarm_visible:
                   sound.play(fade_ms=0)
                start_time_sound=datetime.now()

            x =  element['position'][0]
            y =  element['position'][1]
            W = element['size'][0]
            H = element['size'][1]
            final_x = x+W//2 +15
            final_y = y+H//2 +20
            
            
               
            #εικονα  φωτιας
            image_rect = resized_image_fire.get_rect( center=(final_x,final_y))
            screen.blit(resized_image_fire,image_rect)

            element['lockFire']='1'
            
    if details_visible and  not search_mode:
        details(screen,room_card_list)
    
    #elenxos kathe 25 second an einai ok i epikinonies
    current_time_for_check_connection = datetime.now()
    if(current_time_for_check_connection - last_timer_for_check_connections).seconds >=  time_with_no_check :
        last_timer_for_check_connections = current_time_for_check_connection
        connection_ok_for_print = connection_ok
        connection_ok  = [False, False, False, False, False, False, False, False]
   
    for elem1,elem2 in zip(connection_ok_for_print,connection_pos_new):
        if elem1:
            pygame.draw.circle(screen, (0,255,0), (elem2["position"][0], elem2["position"][1]), 25)
            screen.blit(resized_image_wifi,(elem2["position"][0]-14,elem2["position"][1]-9))
        else:
            pygame.draw.circle(screen, (255,0,0), (elem2["position"][0], elem2["position"][1]), 25)
            screen.blit(resized_image_wifi,(elem2["position"][0]-14,elem2["position"][1]-9))

   
    #emfanisi to keimeno pano apo ta connection
    text_to_print = "CONNECTIONS"
    text_surface = font.render(text_to_print, True, white)
    text_rect = text_surface.get_rect(center =(140,550))
    screen.blit(text_surface, text_rect)

    #emfanisi to keimeno pano apo ta call security
    text_to_print = "CALL SECURITY"
    text_surface = font.render(text_to_print, True, color_security)
    text_rect = text_surface.get_rect(center =(1320,680))
    screen.blit(text_surface, text_rect)

    #emfanisi to keimeno pano apo to lock patient
    text_to_print = "LOCK PATIENT"
    text_surface = font.render(text_to_print, True, color_patient)
    text_rect = text_surface.get_rect(center =(1320,570))
    screen.blit(text_surface, text_rect)
    
    pygame_widgets.update(events)

   
    room1 = list({key for key, value in room_card_list["room1"].items() if value == "patient"})
    room2 = list({key for key, value in room_card_list["room2"].items() if value == "patient"})
    room3 = list({key for key, value in room_card_list["room3"].items() if value == "patient"})
    room6 = list({key for key, value in room_card_list["room6"].items() if value == "patient"})
   
    if id_for_lock :       
        for iter,elem in enumerate(id_for_lock,start=0):                   
            if elem in room6 and not ( elem in room1 or elem in room2 or elem in room3 ) :               
               
                if names_cards[elem] not in name_for_lock :                   
                   name_for_lock.append(names_cards[elem])

                   
                                 
                                  
                   
                   
                  
    #elegxos gia apodrash apo dvmatio
    
    if name_for_lock  and id_for_lock:
        
        #ixos gia alert
        if elapsed_time_lock >= timedelta(seconds=6):
            if not mute_alarm_visible:
              sound_alert.play(fade_ms=0)
            start_time_lock=datetime.now()

        #proto teragono
        surface_lock = {"position": (1180, 380), "size": (300, 350), "color": (255, 160, 160)}
        rect_lock = pygame.Rect(surface_lock["position"], surface_lock["size"])
        inner_lock_surface = pygame.Surface(surface_lock["size"], pygame.SRCALPHA)
        pygame.draw.rect(inner_lock_surface, (200, 200, 200), inner_lock_surface.get_rect(), border_radius=10)
        screen.blit(inner_lock_surface, surface_lock["position"])
        pygame.draw.rect(screen, black, rect_lock, width=5, border_radius=10)

        #deytero  teragono
        surface_lock = {"position": (1220, 450), "size": (220, 250), "color": (0, 160, 160)}
        rect_lock = pygame.Rect(surface_lock["position"], surface_lock["size"])
        inner_lock_surface = pygame.Surface(surface_lock["size"], pygame.SRCALPHA)
        pygame.draw.rect(inner_lock_surface, (255, 255, 255), inner_lock_surface.get_rect(), border_radius=10)
        screen.blit(inner_lock_surface, surface_lock["position"])
        pygame.draw.rect(screen, black, rect_lock, width=5, border_radius=10)
        
        #proto keimeno
        text_lock = "ALARM!"
        font_lock = pygame.font.SysFont('calibri', 40, bold=True)
        textLock_surface = font_lock.render(text_lock, True, red)
        textLock_rect = textLock_surface.get_rect()
        textLock_rect.center = (1330,420)
        screen.blit(textLock_surface, textLock_rect)
        
        #deytero  keimeno
        text_lock = "they leave the room :"
        font_lock = pygame.font.SysFont('calibri', 20, bold=True)
        textLock_surface = font_lock.render(text_lock, True, black)
        textLock_rect = textLock_surface.get_rect()
        textLock_rect.center = (1330,480)
        screen.blit(textLock_surface, textLock_rect)

       
        #keimeno ta onomata 
        for iter,el in enumerate(name_for_lock,start=1):
            text_lock = el
            font_lock = pygame.font.SysFont('calibri', 30, bold=True)
            textLock_surface = font_lock.render(text_lock, True, blue)
            textLock_rect = textLock_surface.get_rect()
            textLock_rect.center = (1325,iter*30 + 500)
            screen.blit(textLock_surface, textLock_rect)

        

    # Ελέγχουμε αν υπάρχουν δεδομένα διαθέσιμα
    try:
        if serIsOk and ser.in_waiting > 0:
           fault_message=False
           data= ser.readline()
           data_string  = data.decode('utf-8')
           
           # αφαιρω τασ μηδενικα στο τελος
           last_colon_index = data_string.rfind(':')    
           if last_colon_index != -1:   
              data_string = data_string[:last_colon_index]

           
           # print(f"Λήφθηκαν δεδομένα: {data_string}")  
           temp_color_security = readCards(data_string,room_card_list,connection_ok,connection_ok_for_print)
           if temp_color_security is not None:
              color_security = temp_color_security
             
          
        elif serIsOk and ser.in_waiting == 0:
            
            if yes_visible :
              
               yes_visible=False
               ser.write('1'.encode())
               #color_security=green
               time.sleep(1) 
              

            elif no_visible:
               
               no_visible=False
               ser.write('0'.encode())
               #color_security=red
               time.sleep(1) 

            elif functions._id_for_delete !=" " and functions._id_for_delete is not None:
                print("Main : id_for_delete :",functions._id_for_delete)
                ser.write(str(functions._id_for_delete).encode())
                functions._id_for_delete =" "
                
                
    except Exception as e:     
            #print(f"Σφάλμα κατά την ανάγνωση δεδομένων: {e}")
            fault_message=True
       
    
    pygame.display.update()




    '''
    #sxediasi mpatarias
    if batteries_visible:
        for element in rooms:
            x =  element['position'][0]
            y =  element['position'][1]
            W = element['size'][0]
            H = element['size'][1]
            final_x = x+W//2
            final_y = y+H//2
    
            #ποσοσστο μπαταριας
            text_surface = font.render('19% ', True, red)
            screen.blit(text_surface, (final_x+60,final_y-10))
            #εικονα  μπαταριας
            image_rect = resized_image_battery.get_rect( center=(final_x,final_y))
            screen.blit(resized_image_battery,image_rect)
    '''
    '''
    if fault_message:
        pygame.draw.circle(screen, (255,0,0), (connection_pos[0], connection_pos[1]), 25)
        screen.blit(resized_image_wifi,(connection_pos[0]-14,connection_pos[1]-9))
        for elem in connection_pos_new:
            pygame.draw.circle(screen, (255,0,0), (elem["position"][0], elem["position"][1]), 25)
            screen.blit(resized_image_wifi,(elem["position"][0]-14,elem["position"][1]-9))
    else:
        pygame.draw.circle(screen, (0,255,0), (connection_pos[0], connection_pos[1]), 25)
        screen.blit(resized_image_wifi,(connection_pos[0]-14,connection_pos[1]-9))
    '''
      