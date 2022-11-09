#include <Windows.h>
#include <iostream>
#include <math.h>

using namespace std;

DWORD pid;
DWORD playerBase = 0x57E254; // This one is static
DWORD enemyBase = 0x06DEAE18;  // change this with enemy's base address

struct {
    DWORD health = 0xEC;
    DWORD horizontal = 0x34;
    DWORD vertical = 0x38;
    DWORD x = 0x4;
    DWORD y = 0x8;
    DWORD z = 0xC;
} offset;


const float pi = 3.14159265;

float player_X, player_Y, player_Z, enemy_X, enemy_Y, enemy_Z, samping, depan, jarak, sudut, tangen, deltaH;

long value;
long temporaryValue;
int memAddress;
long enemyHealth;


float floatRPM(HANDLE process, LPCVOID baseAddr, DWORD ptrOffset) {
    long inputBuffer;
    ReadProcessMemory(process, baseAddr, &inputBuffer, sizeof(inputBuffer), 0);
    memAddress = inputBuffer + ptrOffset;
    
    ReadProcessMemory(process, (LPCVOID)memAddress, &inputBuffer, sizeof(inputBuffer), 0);
    float a = 0;
    *(__int64*)&a = inputBuffer; // cast value from RPM to floating point

    return a;
}

long intRPM(HANDLE process, LPCVOID baseAddr, DWORD ptrOffset) {
    long inputBuffer;
    ReadProcessMemory(process, baseAddr, &inputBuffer, sizeof(inputBuffer), 0);
    memAddress = inputBuffer + ptrOffset;

    ReadProcessMemory(process, (LPCVOID)memAddress, &inputBuffer, sizeof(inputBuffer), 0);

    return inputBuffer;
}

float finalAngle(float pX, float pY, float eX, float eY, float p_angle) {
    float hasilakhir;

    if ((pX < eX) && (pY < eY)) {
        hasilakhir = p_angle;
        
    }
    else {
        if ((pY > eY) && (pX < eX)) {
            hasilakhir = p_angle;
        }
        else {     
            hasilakhir = 180 + p_angle;
        }
        
    }
    return hasilakhir;
}

int main()
{
    HWND hWnd = FindWindowA(0, ("AssaultCube"));
    GetWindowThreadProcessId(hWnd, &pid);
    HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    

    while (true) {

        player_X = floatRPM(pHandle, (LPCVOID)playerBase, offset.x);
        enemy_X = floatRPM(pHandle, (LPCVOID)enemyBase, offset.x);
        player_Y = floatRPM(pHandle, (LPCVOID)playerBase, offset.y);
        enemy_Y = floatRPM(pHandle, (LPCVOID)enemyBase, offset.y);
        player_Z = floatRPM(pHandle, (LPCVOID)playerBase, offset.z);
        enemy_Z = floatRPM(pHandle, (LPCVOID)enemyBase, offset.z);
        enemyHealth = intRPM(pHandle, (LPCVOID)enemyBase, offset.health);
        cout << enemyHealth << "\n";
        if ((enemyHealth > 0) && (enemyHealth < 1000)) {     // if enemy is alive
            samping = player_X - enemy_X;
            depan = player_Y - enemy_Y;
            jarak = sqrt(pow(depan, 2) + pow(samping, 2));
            deltaH = player_Z - enemy_Z;
            tangen = deltaH / jarak;
            sudut = -1 * (atan(tangen) * 180 / pi);

            ReadProcessMemory(pHandle, (LPCVOID)playerBase, &memAddress, sizeof(memAddress), 0);
            memAddress = memAddress + offset.vertical;
            WriteProcessMemory(pHandle, (void*)memAddress, &sudut, sizeof(sudut), 0);

            tangen = depan / samping;

            sudut = finalAngle(player_X, player_Y, enemy_X, enemy_Y, ((atan(tangen) * 180 / pi) + 90));


            //cout << sudut << "\n";
            ReadProcessMemory(pHandle, (LPCVOID)playerBase, &memAddress, sizeof(memAddress), 0);
            memAddress = memAddress + offset.horizontal;
            WriteProcessMemory(pHandle, (void*)memAddress, &sudut, sizeof(sudut), 0);
        }

        //Sleep(1);
    }
    return 0;
}
