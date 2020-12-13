#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

enum EItemPosition {head, neck, armor, rightHand, leftHand, foot};
enum ENumeration {north, east, south, west};

typedef struct Item
{
    char  name[26];
    int   position;
    float weight;
    int   value;
    int   lp_change;
} Item;

typedef struct Monster
{
    char name[21];
    int  lifepoints;
    int  strenght;
    int  dice;
    Item *loot;
} Monster;

typedef struct Room
{   
    int id;
    char        name[26];
    struct Room *nextroom[4];
    int         lpImpact;
    Monster     *monster;
    Item        *loot;
    
} Room;

typedef struct Hero
{
    char name[26];
    int  lifepoints;
    Item *slots[6];
    Item *inventory[10];
    Room *currentRoom;
} Hero;

Item NOTHING = {"Leer", head, 0.0, 0};
Room WALL = {0, "Wand", NULL, 0, NULL, NULL}; 
Item **treasures;
Room **rooms;

int rollDice(int);

void printItem(Item*);
Item* createItem(char*, int, float, int, int);
int readTreasureFile(char*);

void printHero(Hero*);
Hero* createHero(char*, int);
void wearItem(Hero*, Item*);
int addToInventory(Hero*, Item*);
void walkInDirection(Hero*, int);
void setRoom(Hero*, Room*);

Monster* createMonster(char*, int, int, int);
void printMonster(Monster*);
void monsterAddLoot(Monster*, Item*);

Room* createRoom(int, char*, int, Monster*, Item*);
void printRoom(Room*);
void setRoomInDirection(Room*, Room*, int);
Room* getRoomInDirection(Room*, int);
int createMap(char*, int*, Room**);

void printDungeon(Room*, int, int*);
int* getZeroIntArray(int*);
int countRoomsRek(Room*, int*);
int countRoomsWithArray(Room*, int*);
int countRooms(Room*, int);








int main(void)
{
    srand((unsigned int) time(NULL));
    
    int nrOfItems = readTreasureFile("items.txt");

    Room *startRoom = (Room*) malloc(sizeof(Room));

    int nrOfRooms = createMap("rooms.txt", &nrOfItems, &startRoom);
    
    printDungeon(startRoom, -1, NULL);

    int nrOfRoomsRek = countRooms(startRoom, -1);
    printf("countRooms: %d\n", nrOfRoomsRek);

    nrOfRoomsRek = countRoomsWithArray(startRoom, &nrOfRooms);
    printf("nrOfRoomsWithArray: %d\n", nrOfRoomsRek);

    printf("\n");
    
    // only for Windows
    // system("pause");
    
    return 0;
}

int rollDice(int s)
{
    int x = rand() % s + 1;
    return x;
}

/*
 * ---------------
 * Item Funktionen
 * ---------------
 */

void printItem(Item *item)
{
    printf("\n************** Gegenstands-Info **************\n");
    printf("Name:            %s\n", item->name);
    printf("Position:        %d\n", item->position);
    printf("Gewicht:         %.2f\n", item->weight);
    printf("Wert:            %d\n", item->value);
    printf("Veraenderung LP: %d\n", item->lp_change);
    printf("************************************************\n");
}

Item* createItem(char *name, int pos, float wei, int val, int lpc)
{
    Item *newItem = (Item*) malloc(sizeof(Item));
    
    strcpy(newItem->name, name);
    newItem->position = pos;
    newItem->weight = wei;
    newItem->value = val;
    newItem->lp_change = lpc;
    
    return newItem;
}

int readTreasureFile(char *filename)
{
    FILE *fin;
    fin = fopen(filename, "r");
    if (fin == NULL) // muss in der KLausur nicht ueberprueft werden
    {
        printf("readTreasureFile: %s konnt nicht geoeffnet werden\n", filename);
        exit(-1);
    }
    else
    {
        int count = 0;
        int pos;
        float weight;
        int value;
        int lp;
        
        char buffer[256];
        fscanf(fin, "%d\n", &count);

        treasures = (Item**) malloc(count * sizeof(Item*));

        for (int i = 0; i < count; i++)
        {
            fgets(buffer, 256, fin);
            fscanf(fin, "%d,%f,%d,%d\n", &pos, &weight, &value, &lp);
            treasures[i] = createItem(buffer, pos, weight, value, lp);
        }
        return count;
    }

    fclose(fin);
}

/*
 * ---------------
 * Hero Funktionen
 * ---------------
 */

void printHero(Hero *hero)
{
    printf("\n**************** Helden-Info *****************\n");
    printf("Name:         %s\n", hero->name);
    printf("Lebenspunkte: %d\n", hero->lifepoints);

    if (hero->currentRoom == NULL)
        printf("Raum:         kein Raum\n");
    else
        printf("Raum:         %s\n", hero->currentRoom->name);

    printf("Slots:\n");
    printf("\t\tKopf:        %s\n", hero->slots[head]->name);
    printf("\t\tHals:        %s\n", hero->slots[neck]->name);
    printf("\t\tRuestung:    %s\n", hero->slots[armor]->name);
    printf("\t\tLinke Hand:  %s\n", hero->slots[leftHand]->name);
    printf("\t\tRechte Hand: %s\n", hero->slots[rightHand]->name);
    printf("\t\tFuesse:      %s\n", hero->slots[foot]->name);
    printf("Inventar:\n");

    for (int i = 0; i < 10; i++)
    {
        printf("\t\t%2d. Item:    %s\n",(i + 1), hero->inventory[i]->name);
    }

    printf("************************************************\n");
}

Hero* createHero(char* name, int lp)
{
    Hero *newHero = (Hero*) malloc(sizeof(Hero));
    
    strcpy(newHero->name, name);
    newHero->lifepoints = lp;
    
    // alle slots (1 - 6)auf NOTHING setzen
    for (int i = 0; i < 6; i++)
    {
        newHero->slots[i] = &NOTHING;
    }
    
    // alle inventory (1 - 10) auf NOTHING setzen
    for (int i = 0; i < 10; i++)
    {
        newHero->inventory[i] = &NOTHING;
    }
    
    newHero->currentRoom = NULL;
    
    return newHero;
}

void wearItem(Hero *hero, Item *item)
{
    if (strcmp(hero->slots[item->position]->name, NOTHING.name) == 0)
    {
        hero->slots[item->position] = item;
    }
    else
    {
        // flag fuer freier Platz gefunden
        int isFree = 0;
        
        for (int i = 0; i < 10; i++)
        {
            // isFree == 1 wenn freier Platz gefunden wurde
            isFree = (strcmp(hero->inventory[i]->name, NOTHING.name) == 0);
            
            if (isFree)
            {
                hero->inventory[i] = hero->slots[item->position];
                hero->slots[item->position] = item;
                break;
            }
        }
    }
}

int addToInventory(Hero *hero, Item *item)
{
    int flag = -1;
    
    for (int i = 0; i < 10; i++)
    {
        // strcmp(char *s1, chat *s2) == 0 wenn s1 == s2
        if (strcmp(hero->inventory[i]->name, NOTHING.name) == 0)
        {
            hero->inventory[i] = item;
            flag = 1;
            break;
        }
    }
    
    return flag;
}

void walkInDirection(Hero *hero, int direction)
{
    if (hero->currentRoom == NULL)
    {
        printf("\nwalkInDirection: Held hat keinen Raum\n");
    }
    else
    {
        Room *walkTo = getRoomInDirection(hero->currentRoom, direction);
        
        if (walkTo != NULL)
        {
            setRoom(hero, walkTo);
        }
        else
        {
            printf("\n\nAchtung: Da ist kein Raum!\n\n");
        }
    }
}

void setRoom(Hero *hero, Room *room)
{
    hero->currentRoom = room;
}

/*
 * ------------------
 * Monster Funktionen
 * ------------------
 */

Monster* createMonster(char* name, int lp, int stren, int d)
{
    Monster *newMonster = (Monster*) malloc(sizeof(Monster));
    
    strcpy(newMonster->name, name);
    newMonster->lifepoints = lp;
    newMonster->strenght = stren;
    newMonster->dice = d;
    newMonster->loot = &NOTHING;
    
    return newMonster;
}

void printMonster(Monster *monster)
{
    printf("\n**************** Monster-Info ****************\n");
    printf("Name:         %s\n", monster->name);
    printf("Lebenspunkte: %d\n", monster->lifepoints);
    printf("Staerke:      %d\n", monster->strenght);
    printf("Wuerfel:      %d-seitig\n", monster->dice);
    printf("Loot:         %s\n", monster->loot->name);
    printf("************************************************\n");
}

void monsterAddLoot(Monster *monster, Item *item)
{
    monster->loot = item;
}

/*
 * ---------------
 * Raum Funktionen
 * ---------------
 */

Room* createRoom(int id, char* name, int lpImp, Monster *monster, Item *item)
{
    Room *newRoom = (Room*) malloc(sizeof(Room));

    newRoom->id = id;
    strcpy(newRoom->name, name);
    
    for (int i = 0; i < 4; i++)
    {
        newRoom->nextroom[i] = &WALL;
    }
    
    newRoom->lpImpact = lpImp;
    newRoom->monster = monster;
    newRoom->loot = item;
    return newRoom;
}

void printRoom(Room *room)
{
    printf("\n***************** Raum-Info ******************\n");
    printf("Name: %s\n", room->name);
    printf("Nachbarraeume:\n");
    printf("\t\t\t  Nord: %s\n", room->nextroom[north]->name);
    printf("\t\t\t  Ost : %s\n", room->nextroom[east]->name);
    printf("\t\t\t  Sued: %s\n", room->nextroom[south]->name);
    printf("\t\t\t  west: %s\n", room->nextroom[west]->name);
    
    if (room->monster == NULL)
    {
        printf("Monster: kein Monster\n");
    }
    else
    {
        printf("Monster: %s\n", room->monster->name);
    }
    
    printf("Schatz:  %s\n", room->loot->name);
    printf("LP_Impact: %d\n", room->lpImpact);
    printf("************************************************\n");

}

void setRoomInDirection(Room *room_1, Room *room_2, int direction)
{
    room_1->nextroom[direction] = room_2;

    room_2->nextroom[((int) direction + 2) % 4] = room_1;
}

Room* getRoomInDirection(Room *room, int direction)
{
    Room *inDirection = room->nextroom[direction];
    
    if (inDirection->name != WALL.name)
    {
        return room->nextroom[direction];
    }
    else
    {
        return NULL;
    }
}

/*
 *****************
 * createDungeon *
 *****************
 * creates a Dungeon from a "rooms.txt"
 * returns the number of rooms in the Dungeon ans sets the "Entrance" as startRoom
 */

int createMap(char* filename, int *n, Room **startRoom)
{
    FILE *fin;
    fin = fopen(filename, "r");
    if (fin == NULL)
    {
        printf("\ncreateMap: oeffnen der Datei %s fehlgeschlagen\n", filename);
        exit(-1);
    }
    else
    {
        // Anzahl der Raeume einlesen
        int count = 0;
        fscanf(fin, "%d\n", &count);

        // Speicher für "count" Raeume allokieren
        rooms = (Room**) malloc(count * sizeof(Room*));

        char buffer[256];
        char name[256];
        char c;
        int id;
        int lpchange;

        // ID, Name, lpchange Zeile fuer Zeile auslesene
        for (int i = 0; i < count; i++)
        {
            // id einlesen + restlicher String
            fscanf(fin, "%d,\"%s", &id, buffer);
            // Name von der Zahl isolieren
            c = buffer[0];
            // Zeahler fuer Schleife um Komma + Index fuer Stelle nach dem Komma zu suchen
            int j = 0;
            // nach der Schleife hat j den Index der Stelle nach dem Komma
            while (c != ',')
            {
                // c = naechstes Zeichen
                c = buffer[j];
                j++;
            }
            // Nullbyte beim Komma setzen, sodass String dort ausfhoert
            // und in "name" speichern
            // j - 2: weil am Ende ", steht und j die Stelle nach dem Komma ist
            buffer[j - 2] = '\0';
            strcpy(name, buffer);
            // string nach dem Komma als int in lpchange speichern 
            sscanf(buffer + j, "%d\n", &lpchange);
            rooms[i] = createRoom(id, name, lpchange, NULL, treasures[rand() % (*n)]);
        }
        fgets(buffer, 256, fin);
        int currentRoom;
        int dNorth;
        int dEast;
        int dSouth;
        int dWest;
        for (int i = 0; i < count; i++)
        {
            fscanf(fin, "%d,%d,%d,%d,%d\n", &currentRoom, &dNorth, &dEast, &dSouth, &dWest);
            if (dNorth != 0)
                setRoomInDirection(rooms[currentRoom - 1], rooms[dNorth - 1], north);
            if (dEast != 0)
                setRoomInDirection(rooms[currentRoom - 1], rooms[dEast - 1], east);
            if (dSouth != 0)
                setRoomInDirection(rooms[currentRoom - 1], rooms[dSouth - 1], south);
            if (dWest != 0)
                setRoomInDirection(rooms[currentRoom - 1], rooms[dWest - 1], west);
        }
        
        // Startraum zurückgeben, weil Eingang nicht immer rooms[0] ist
        fclose(fin);
        *startRoom = rooms[0];
        return count;
    }
}

void printDungeon(Room *room, int direction, int *rekDepth)
{
    printRoom(room);

    for (int i = north; i <= west; i++)
    {
        if (i != direction && getRoomInDirection(room, i) != NULL)
        {
            printDungeon(getRoomInDirection(room, i), (i + 2) % 4, rekDepth + 1);
        }
    }
}

int* getZeroIntArray(int *size)
{
    int *newArray = (int*) malloc((*size) * sizeof(int));
    for (int i = 0; i < *size; i++)
        newArray[i] = 0;
    return newArray;
}

int countRoomsRek(Room* room, int* wasIn)
{
    Room *roomInDirection = (Room*) malloc(sizeof(Room*));
    int sum = 0;
    wasIn[room->id - 1] = 1;

    
    for (int i = north; i <= west; i++)
    {
        // raum in Richtung i speicher --> uebersichtlicher
        roomInDirection = getRoomInDirection(room, i);

        // wenn in Richtung i ein Raum ist ...
        if (roomInDirection != NULL)
        {
            // und man noch nicht in diesem Raum war
            if (wasIn[roomInDirection->id - 1] != 1)
            {
                // dann markieren, dass er in diesem Raum war ..
                wasIn[roomInDirection->id - 1] = 1;
                // zaehle alle Raeume, die an diese Raum angrenzen und in denen man noch nicht war
                sum += countRoomsRek(roomInDirection, wasIn);
            }            
        }
    }
    // free(roomInDirection);
    return (1 + sum);
}

int countRoomsWithArray(Room* startRoom, int *nrOfRooms)
{
    // erstelle int-Array mit nrOfRooms Stellen; alle Stellen = 0 
    int *wasIn = getZeroIntArray(nrOfRooms);
    // Rueckgabe der Anzahl der Raeume
    return countRoomsRek(startRoom, wasIn);
}

int countRooms(Room *room, int direction)
{
    int sum = 0;

    for (int i = north; i <= west; i++)
    {
        if (i != direction && getRoomInDirection(room, i) != NULL)
        {
            sum += countRooms(getRoomInDirection(room, i), (i + 2) % 4);
        }
    }

    return (1 + sum);
}