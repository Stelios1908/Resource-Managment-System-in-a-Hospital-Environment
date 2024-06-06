gray = (200, 200, 200)
black = (0, 0, 0)


rooms = [
    {'position': (100, 100), 'size': (350, 300),'fire':0,'lockFire':0},
    {'position': (450, 100), 'size': (350, 300),'fire':0,'lockFire':0},
    {'position': (800, 100), 'size': (350, 300),'fire':0,'lockFire':0},
    {'position': (275, 500), 'size': (350, 250),'fire':0,'lockFire':0},
    {'position': (625, 500), 'size': (350, 250),'fire':0,'lockFire':0},
    {'position': (100, 400), 'size': (1050, 100),'fire':0,'lockFire':0},#διαδρομος

]


corridors = [
    #εξωτερικοι τοιχοι
    {'start': (100,100), 'end': (1150,100),'color':black,'size':10},
    {'start': (100, 97), 'end': (100, 500),'color':black,'size':10},
    {'start': ( 97,500), 'end': (275, 500),'color':black,'size':10},
    {'start': (275,500), 'end': (275, 750),'color':black,'size':10},
    {'start': (272,750), 'end': (975, 750),'color':black,'size':10},
    {'start': (975,750), 'end': (975, 500),'color':black,'size':10},
    {'start': (975,500), 'end': (1155, 500),'color':black,'size':10},
    {'start': (1150,495),'end': (1150,100),'color':black,'size':10},

    #πορτες
    {'start': (230,398), 'end': (320, 398),'color':gray,'size':10,'alarm':0,'lockAlarm':0},
    {'start': (570,398), 'end': (660, 398),'color':gray,'size':10,'alarm':0,'lockAlarm':0},
    {'start': (930,398), 'end': (1020,398),'color':gray,'size':10,'alarm':0,'lockAlarm':0},
    {'start': (415,500), 'end': (485, 500),'color':gray,'size':10,'alarm':0,'lockAlarm':0},
    {'start': (765,500), 'end': (835, 500),'color':gray,'size':10,'alarm':0,'lockAlarm':0},
    {'start': (1150,420), 'end':(1150, 480),'color':gray,'size':10,'alarm':0,'lockAlarm':0},
    
    #παραθυρα
    {'start': (230,100), 'end':(320, 100),'color' :black,'size':3,'alarm':0,'lockAlarm':0},
    {'start': (570,100), 'end':(660, 100),'color': black,'size':3,'alarm':0,'lockAlarm':0},
    {'start': (930,100), 'end':(1020, 100),'color':black,'size':3,'alarm':0,'lockAlarm':0},
    {'start': (415,750), 'end':(485, 750),'color': black,'size':3,'alarm':0,'lockAlarm':0},
    {'start': (765,750), 'end':(835, 750),'color': black,'size':3,'alarm':0,'lockAlarm':0},
    {'start': (101,420), 'end':(101, 480),'color': black,'size':3,'alarm':0,'lockAlarm':0}
]
