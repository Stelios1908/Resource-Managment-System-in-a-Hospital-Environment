import pygame


surfaces_for_buttons =[
    {"position": (60, 10),   "size": (1090, 70), "color": (0, 128, 255)},
    {"position": (1160, 10), "size": (300, 80),  "color": (0, 128, 255)},
    {"position": (1160, 70), "size": (340, 60),  "color": (0, 128, 255)},
    {"position": (1160, 140), "size": (340, 230), "color": (0, 128, 255)},
    {"position": (30, 525), "size": (220, 230), "color": (0, 128, 255)},
    {"position": (1150, 660), "size": (350, 100), "color": (160, 160, 160)},
    {"position": (1150, 550), "size": (350, 100), "color": (160, 160, 160)},
]

button_data = [
    {'text': 'DOCTORS', 'color': (0, 122, 0), 'text_color': (255, 255, 255)},
    {'text': 'NURSES', 'color': (255, 0, 80), 'text_color': (255,255,255)},
    {'text': 'PATIENTS', 'color': (0, 0, 255), 'text_color': (255,255,255)},
    {'text': 'DEVICES', 'color': (204, 102, 0), 'text_color': (255,255,255)},
    {'text': 'ALL', 'color': (64, 64, 64), 'text_color': (255,255,255)},
    {'text': 'CLEAR', 'color': (255, 165, 0), 'text_color': (255,255,255)},
    {'text': 'HUMIDITY', 'color': (0, 0, 255), 'text_color': (255,255,255)},
    {'text': 'TEMPERATURE', 'color': (255, 0, 0), 'text_color': (255,255,255)},   
    {'text': 'MUTE_ALARM', 'color': (120, 110, 10), 'text_color': (255,255,255)},  
    {'text': 'CLEAR_ALARM', 'color': (40, 110, 10), 'text_color': (255,255,255)},  
    {'text': 'LOG_FILE', 'color': (17, 170, 10), 'text_color': (255,255,255)},
    {'text': 'CLEAR_SEARCH', 'color': (170, 170, 10), 'text_color': (255,255,255)},
    {'text': 'DETAILS', 'color': (0, 0, 0), 'text_color': (255,255,255)},
    {'text': 'YES', 'color': (0, 255, 0), 'text_color': (255,255,255)},
    {'text': 'NO', 'color': (255, 0, 0), 'text_color': (255,255,255)},

]



# Δημιουργία λίστας για τα κουμπιά
buttons = []
for i, data in enumerate(button_data):
    if i==6 or i==7 : #hum and temp
       button = pygame.Rect(1510 + 170*(i-8), 150, 150, 50)
       buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})    
    elif i ==8  or i==9: #mute and clear alarm
       button = pygame.Rect(1510 + 170*(i-10), 260, 150, 50)
       buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})
    elif i == 10: # log
       button = pygame.Rect(1510 + 170*(i-12), 320, 320, 40)
       buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})
    elif i==11: #clear search
        button = pygame.Rect(1510 + 170*(i-13), 80, 320, 40)
        buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})
    elif i==12: #details
        button = pygame.Rect(1510 + 170*(i-14), 210, 320, 40)
        buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})
    elif i==13 or i==14: #call security
        button = pygame.Rect(1510 + 170*(i-15), 700, 150, 40)
        buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})
    else:
       button = pygame.Rect(80 + i * 180, 20, 150, 50)
       buttons.append({'rect': button, 'text': data['text'], 'text_surface': None, 'text_rect': None, 'color': data['color'], 'text_color': data['text_color']})


