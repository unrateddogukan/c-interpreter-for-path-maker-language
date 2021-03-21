#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
#include <dirent.h>

int main() {

    char kaynakDosyaAdi[31] = {}; //input olarak girilen kaynak dosya isminin tutuldugu char array.
    char okunan[1023] = {}; //kaynak dosya iceriginin tutuldugu char array.
    char currentDir[255] = {}; //mevcut dizin adresinin tutuldugu char array.
    char currentDirTemp[255] = {}; //mevcut dizinde degisiklik yapilirken kullanilan temp.
    char makeDirTemp[255] = {}; //'make' ve if/ifnot islemleri yapilirken kullanilan temp.

    int i = 0, j, k, yeniDirUzunluk, dirBaslangic, slashKontrol, make = 0, go = 0, iff = 0, ifnot = 0, ifSorgusu, ifnotSorgusu;

    DWORD toFindCurDir[255];
    DIR *checkDir;

    GetCurrentDirectory(toFindCurDir, currentDir); //mevcut dizin adresi alinip, 'currentDir' char array'inde tutuluyor.

//kaynak dosyanın ismi alinip, alinan isimdeki dosyanin icerigi 'okunan' char array'ine kopyalaniyor.
    printf("Kaynak Dosyanin Adini Giriniz: ");
    scanf("%s", kaynakDosyaAdi);
    strcat(kaynakDosyaAdi, ".pmk.txt");

    FILE *dosya1;

    if ((dosya1 = fopen(kaynakDosyaAdi,"r")) == NULL) {
        printf("\nhata: dosya bulunamadi.\n");
    }
    else {
        while (!feof(dosya1)) {
            okunan[i] = fgetc(dosya1);
            i++;
        }
    }

    fclose(dosya1);

    printf("\n------ program basi mevcut dizin: %s\n ", currentDir);
    printf("\nkaynak pmk dosyasi: \n%s\n ", okunan);
    printf("\n---------------------------------------------\n");

// kaynak dosyadaki direktifler, 'okunan' char array'ine kopyalanmisti.
// bu for dongusu de 'okunan' char array'indeki her karaktere gore Path Maker dilinin getirilerini gerceklestiriyor.
    for (i = 0; i < strlen(okunan); i++) {

        if (isspace(okunan[i])) continue; //komutlar arasındaki bosluklar ihmal ediliyor.

        // dongude 'make' kelimesi okundugunda, gereken islemi gerçekleştirebilmek icin int degisken 'make'e +1 ekleniyor.
        if (okunan[i] == 'm' && okunan[i+1] == 'a' && okunan[i+2] == 'k' && okunan[i+3] == 'e'){
            make++;
            i += 3;
            continue;
        }

        // dongude 'go' kelimesi okundugunda, gereken islemi gerçekleştirebilmek icin int degisken 'go'ya +1 ekleniyor.
        if (okunan[i] == 'g' && okunan[i+1] == 'o'){
            go++;
            i += 1;
            continue;
        }

        // dongude 'if' kelimesi okundugunda, gereken islemi gerçekleştirebilmek icin int degisken 'iff'e +1 ekleniyor.
        if (okunan[i] == 'i' && okunan[i+1] == 'f'){
            iff++;
            ifSorgusu = 1;
            i += 1;
            continue;
        }

        // dongude 'ifnot' kelimesi okundugunda, gereken islemi gerçekleştirebilmek icin int degisken 'ifnot'a +1 ekleniyor.
        if (okunan[i] == 'i' && okunan[i+1] == 'f' && okunan[i+2] == 'n' && okunan[i+3] == 'o' && okunan[i+4] == 't'){
            ifnot++;
            ifnotSorgusu = 1;
            i += 4;
            continue;
        }

        // path ifadesinin basladigini belirlemek icin '<' karakterinin okunmasi sarti araniyor.
        if (okunan[i] == '<'){

            i++;

            while (isspace(okunan[i])) i++; // bosluk karakterler ihmal ediliyor.

            // path ifadeleri '/' oparatoru ile baslayamayacagi icin, bu karakter okundugunda hata veriliyor.
            if (okunan[i] == '/') {
                printf("\nhata: path ifadesi '/' operatoru ile baslayamaz veya bitemez.\n");
                exit(0);
            }

            // dongunun basinda 'make' kelimesi okundugunda int degiskeni 'make'e +1 eklemistik. burada da 'make' kelimesinin
            // okunup okunmadigini int degiskeni make'in degerine bakarak anliyoruz. bu deger tek sayi ise okunmus, cift sayi ise okunmamistir.
            if (make % 2 != 0) {

                strcpy(makeDirTemp, currentDir); // mevcut dizin adresini makeDirTemp char array'ine kopyaliyoruz.

                while(1) {

                    // okunan karakter '*' ise, mevcut dizin adresini kopyaladigimiz makeDirTemp char array'ini, bir ust dizin olacak sekilde guncelliyoruz.
                    if (okunan[i] == '*') {

                        for (j = 0; j < strlen(makeDirTemp); j++) {

                            if (makeDirTemp[j] == '\\') {

                                yeniDirUzunluk = j;
                            }
                        }

                        makeDirTemp[yeniDirUzunluk] = '\0';

                        i++;
                    }

                    while (isspace(okunan[i])) i++;

                    // 100. satirdaki while dongusu '/' ve sonrasinde '*' karakterleri okundugu surece devam ediliyor.
                    // bu sart saglanmadiginda donguden cikiliyor.
                    if (okunan[i] == '/') {

                        i++;

                        while (isspace(okunan[i])) i++;

                        if (okunan[i] == '*') {
                            continue;
                        } else {
                            break;
                        }
                    }

                    break;
                }



                while (okunan[i] != '>') { // path ifadesinin sonuna geldigimizi '<' karakterini okuyarak farkediyoruz.

                    char *dirNameTemp = calloc(31, sizeof(char)); // kaynak dosyadan okunan dizin isimlerinin tutulacagi calloc char array'i.

                    j = 0;

                    while (1) {

                        if (ispunct(okunan[i]) && !(okunan[i] == '_')) {

                            printf("\nhata: dizin isminde gecersiz karakter.\n");
                            exit(0);
                        }

                        if ((j == 0) && isdigit(okunan[i])) {

                            printf("\nhata: dizin isimleri harf veya '*' disinda bir karakterle baslayamaz.\n");
                            exit(0);
                        }

                        k = i;

                        if (isspace(okunan[k])) {

                            while (isspace(okunan[k])) k++;

                            if (isalpha(okunan[k])) {
                                printf("\nhata: dizin ismi icerisinde bosluk bulunamaz.\n");
                                exit(0);
                            }
                        }

                        dirNameTemp[j] = okunan[i]; // kaynak dosyadan okunan dizin isimleri, dirNameTemp calloc char array'ine kopyalaniyor.

                        i++;
                        j++;

                        // 146. satirdaki while dongusu dizin isminin bittigini farkedene kadar devam ediyor.
                        // bunu da '/' veya '<' karakterlerini okudugumuzda fark ediyoruz.
                        if (isspace(okunan[i]) || (okunan[i] == '/') || (okunan[i] == '>')) break;

                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') i++;

                    while (isspace(okunan[i])) i++;

                    strcat(makeDirTemp, "\\");
                    strcat(makeDirTemp, dirNameTemp); // kaynak dosyada okunan dizin ismi, mevcut dizin adresinin tutuldugu temp'e ekleniyor.

                    free(dirNameTemp);

                    checkDir = opendir(makeDirTemp);

                    if (checkDir == NULL) {
                        mkdir(makeDirTemp);
                        printf("\n'make' komutu ile olusturulan dizin: '%s'\n", makeDirTemp);
                    } else {
                        printf("\n'make' komutu ile olusturmak istenilen '%s' hali hazirda mevcut.\n", makeDirTemp);
                    }
                }

                k = i;

                k--;

                while (isspace(okunan[k])) k--;

                if (okunan[k] == '/') {
                    printf("\nhata: path ifadeleri '/' operatoru ile baslayamaz veya bitemez.\n");
                    exit(0);
                }

                i++;

                while (isspace(okunan[i])) i++;

                make++;

                if (okunan[i] != ';') {
                    printf("\nhata: 'make' islemi sonrasinda ';' isareti eksikligi.");
                    break;
                }
            }

            // dongunun basinda 'go' kelimesi okundugunda int degiskeni 'go'ya +1 eklemistik. burada da 'go' kelimesinin
            // okunup okunmadigini int degiskeni 'go'nun degerine bakarak anliyoruz. bu deger tek sayi ise okunmus, cift sayi ise okunmamistir.
            // 'go' komutunun calisma prensibi 'make'e benzer, tek fark 'make' eger yoksa okunan dizini olusturur, 'go' eger varsa okunan dizini mevcut dizin yapar.
            if (go % 2 != 0) {

                strcpy(currentDirTemp, currentDir);

                while(1) {

                    if (okunan[i] == '*') {

                        for (j = 0; j < strlen(currentDirTemp); j++) {

                            if (currentDirTemp[j] == '\\') {

                                yeniDirUzunluk = j;
                            }
                        }

                        currentDirTemp[yeniDirUzunluk] = '\0';

                        strcpy(currentDir, currentDirTemp);
                        printf("\n 'go' komutu ile gecilen yeni mevcut dizin: '%s'\n", currentDir);

                        i++;
                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') {

                        i++;

                        while (isspace(okunan[i])) i++;

                        if (okunan[i] == '*') {
                            continue;
                        } else {
                            break;
                        }
                    }

                    break;
                }

                while (okunan[i] != '>') {

                    char *dirNameTemp = calloc(31, sizeof(char));

                    j = 0;

                    while (1) {

                        if (ispunct(okunan[i]) && !(okunan[i] == '_')) {

                            printf("\nhata: dizin isminde geçersiz karakter.\n");
                            exit(0);
                        }

                        if ((j == 0) && isdigit(okunan[i])) {

                            printf("\nhata: dizin isimleri harf veya '*' disinda bir karakterle baslayamaz.\n");
                            exit(0);
                        }

                        k = i;

                        if (isspace(okunan[k])) {

                            while (isspace(okunan[k])) k++;

                            if (isalpha(okunan[k])) {
                                printf("\nhata: dizin ismi icerisinde bosluk bulunamaz.\n");
                                exit(0);
                            }
                        }

                        dirNameTemp[j] = okunan[i];

                        i++;
                        j++;

                        if (isspace(okunan[i]) || (okunan[i] == '/') || (okunan[i] == '>')) break;

                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') i++;

                    while (isspace(okunan[i])) i++;

                    strcat(currentDirTemp, "\\");
                    strcat(currentDirTemp, dirNameTemp);

                    checkDir = opendir(currentDirTemp);

                    if (checkDir == NULL) {
                        printf("\nhata: secilen dizin mevcut degil, mevcut dizin degistirilemedi.\n");
                    } else {
                        strcpy(currentDir, currentDirTemp);
                        printf("\n 'go' komutu ile degistirilen yeni mevcut dizin: '%s'\n", currentDir);
                    }
                }

                k = i;

                k--;

                while (isspace(okunan[k])) k--;

                if (okunan[k] == '/') {
                    printf("\nhata: path ifadeleri '/' operatoru ile baslayamaz veya bitemez.\n");
                    exit(0);
                }

                i++;

                while (isspace(okunan[i])) i++;

                go++;

                if (okunan[i] != ';') {
                    printf("\nhata: 'go' islemi sonrasinda ';' isareti eksikligi.\n");
                    break;
                }
            }

            // dongunun basinda 'if' kelimesi okundugunda int degiskeni 'iff'e +1 eklemistik. burada da 'if' kelimesinin
            // okunup okunmadigini int degiskeni 'iff'in degerine bakarak anliyoruz. bu deger tek sayi ise okunmus, cift sayi ise okunmamistir.
            if (iff % 2 != 0) {

                strcpy(makeDirTemp, currentDir);

                while(1) {

                    if (okunan[i] == '*') {

                        for (j = 0; j < strlen(makeDirTemp); j++) {

                            if (makeDirTemp[j] == '\\') {

                                yeniDirUzunluk = j;
                            }
                        }

                        makeDirTemp[yeniDirUzunluk] = '\0';

                        i++;
                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') {

                        i++;

                        while (isspace(okunan[i])) i++;

                        if (okunan[i] == '*') {
                            continue;
                        } else {
                            break;
                        }
                    }

                    break;
                }

                while (okunan[i] != '>') {

                    char *dirNameTemp = calloc(31, sizeof(char));

                    j = 0;

                    while (1) {

                        if (ispunct(okunan[i]) && !(okunan[i] == '_')) {

                            printf("\nhata: dizin isminde geçersiz karakter.\n");
                            exit(0);
                        }

                        if ((j == 0) && isdigit(okunan[i])) {

                            printf("\nhata: dizin isimleri harf veya '*' disinda bir karakterle baslayamaz.\n");
                            exit(0);
                        }

                        k = i;

                        if (isspace(okunan[k])) {

                            while (isspace(okunan[k])) k++;

                            if (isalpha(okunan[k])) {
                                printf("\nhata: dizin ismi icerisinde bosluk bulunamaz.\n");
                                exit(0);
                            }
                        }

                        dirNameTemp[j] = okunan[i];

                        i++;
                        j++;

                        if (isspace(okunan[i]) || (okunan[i] == '/') || (okunan[i] == '>')) break;
                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') i++;

                    while (isspace(okunan[i])) i++;

                    strcat(makeDirTemp, "\\");
                    strcat(makeDirTemp, dirNameTemp);

                    free(dirNameTemp);

                    checkDir = opendir(makeDirTemp);

                    ifSorgusu = 1;

                    if (checkDir == NULL) {

                        printf("\nif ile varligi sorgulanan dizin yok.\n");
                        ifSorgusu = 0;

                        // if'ten sonra gelen path ifadesindeki dizin yoksa, kod, bi sonraki komut okunana kadar islem yapmaz.
                        while(okunan[i] != ';') {
                            break;
                        }

                    } else {
                        printf("\nif ile varligi sorgulanan dizin var.\n");
                    }
                }

                k = i;

                k--;

                while (isspace(okunan[k])) k--;

                if (okunan[k] == '/') {
                    printf("\nhata: path ifadeleri '/' operatoru ile baslayamaz veya bitemez.\n");
                    exit(0);
                }

                i++;

                while (isspace(okunan[i])) i++;

                // if'ten sonra gelen path ifadesindeki dizin yoksa ve komut blok ise, kod, blok bitip bir sonraki komut okunana kadar islem yapmaz.
                if (okunan[i] == '{') {

                    i++;
                    while (isspace(okunan[i])) i++;

                    if (ifSorgusu == 0) {

                        while(okunan[i] != '}') i++;
                    }
                }

                iff++;

                i--;
            }

            // dongunun basinda 'ifnot' kelimesi okundugunda int degiskeni 'ifnot'a +1 eklemistik. burada da 'ifnot' kelimesinin
            // okunup okunmadigini int degiskeni 'ifnot'in degerine bakarak anliyoruz. bu deger tek sayi ise okunmus, cift sayi ise okunmamistir.
            // ifnot sorgulamasinin icerigi if ile buyuk olcude aynidir. tek fark if'te dizinin oldugu, ifnot'ta olmadigi durum aranir.
            if (ifnot % 2 != 0) {

                strcpy(makeDirTemp, currentDir);

                while(1) {

                    if (okunan[i] == '*') {

                        for (j = 0; j < strlen(makeDirTemp); j++) {

                            if (makeDirTemp[j] == '\\') {

                                yeniDirUzunluk = j;
                            }
                        }

                        makeDirTemp[yeniDirUzunluk] = '\0';

                        i++;
                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') {

                        i++;

                        while (isspace(okunan[i])) i++;

                        if (okunan[i] == '*') {
                            continue;
                        } else {
                            break;
                        }
                    }

                    break;
                }

                while (okunan[i] != '>') {

                    char *dirNameTemp = calloc(31, sizeof(char));

                    j = 0;

                    while (1) {

                        if (ispunct(okunan[i]) && !(okunan[i] == '_')) {

                            printf("\nhata: dizin isminde geçersiz karakter.\n");
                            exit(0);
                        }

                        if ((j == 0) && isdigit(okunan[i])) {

                            printf("\nhata: dizin isimleri harf veya '*' disinda bir karakterle baslayamaz.");
                            exit(0);
                        }

                        k = i;

                        if (isspace(okunan[k])) {

                            while (isspace(okunan[k])) k++;

                            if (isalpha(okunan[k])) {
                                printf("\nhata: dizin ismi icerisinde bosluk bulunamaz.\n");
                                exit(0);
                            }
                        }

                        dirNameTemp[j] = okunan[i];

                        i++;
                        j++;

                        if (isspace(okunan[i]) || (okunan[i] == '/') || (okunan[i] == '>')) break;

                    }

                    while (isspace(okunan[i])) i++;

                    if (okunan[i] == '/') i++;

                    while (isspace(okunan[i])) i++;

                    strcat(makeDirTemp, "\\");
                    strcat(makeDirTemp, dirNameTemp);

                    free(dirNameTemp);

                    checkDir = opendir(makeDirTemp);

                    ifnotSorgusu = 1;

                    if (checkDir != NULL) {

                        ifnotSorgusu = 0;
                        printf("\nifnot ile varligi sorgulanan dizin var.\n");
                        while(okunan[i] != ';') {
                            break;
                        }
                    } else {
                        printf("\nifnot ile varligi sorgulanan dizin yok..\n");
                    }
                }

                k = i;

                k--;

                while (isspace(okunan[k])) k--;

                if (okunan[k] == '/') {
                    printf("\nhata: path ifadeleri '/' operatoru ile baslayamaz veya bitemez.\n");
                    exit(0);
                }

                i++;

                while (isspace(okunan[i])) i++;

                if (okunan[i] == '{') {

                    i++;
                    while (isspace(okunan[i])) i++;

                    if (ifnotSorgusu == 0) {

                        while(okunan[i] != '}') i++;
                    }
                }

                ifnot++;

                i--;
            }
        }
    }

    printf("\n---------------------------------------------\n");

    printf("\n------ program sonu mevcut dizin: %s\n ", currentDir);

    return 0;
}
