#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h> 
#include <windows.h> 

using namespace std;

// Ekran boyuatlar�
const int GENISLIK = 30;
const int YUKSEKLIK = 20;

// Temel s�n�f: OyunNesnesi
class OyunNesnesi {
protected:
    int x, y; // Konum bilgisi
public:
    OyunNesnesi(int x, int y) : x(x), y(y) {}
    virtual void hareketEt() = 0; // Soyut hareket fonksiyonu
    int getX() const { return x; }
    int getY() const { return y; }
    void konumAta(int yeniX, int yeniY) {
        x = yeniX;
        y = yeniY;
    }
};

// Uzay Gemisi (Spaceship) S�n�f�
class UzayGemisi : public OyunNesnesi {
public:
    UzayGemisi(int x, int y) : OyunNesnesi(x, y) {}

    void hareketEt(char yon) {
        if (yon == 'a' && x > 1) x--; // Sol
        if (yon == 'd' && x < GENISLIK - 4) x++; // Sa�
    }

    void hareketEt() override {} // Bo� implementasyon
};

// Mermi (Bullet) S�n�f�
class Mermi : public OyunNesnesi {
public:
    Mermi(int x, int y) : OyunNesnesi(x, y) {}

    void hareketEt() override {
        y--; // Mermi yukar�ya do�ru hareket eder
    }
};

// D��man (Enemy) S�n�f�
class Dusman : public OyunNesnesi {
public:
    Dusman(int x, int y) : OyunNesnesi(x, y) {}

    void hareketEt() override {
        y++; // D��man a�a��ya do�ru hareket eder
    }
};

// Oyun S�n�f�
class Oyun {
private:
    UzayGemisi* oyuncu;
    vector<Mermi*> mermiler;
    vector<Dusman*> dusmanlar;
    bool calisiyor;
    int dusmanHizSayaci = 0;
    int yokEdilenDusmanlar = 0; // Yok edilen d��man say�s�n� tutar

    void ekranTemizle() {
        system("cls"); // Konsolu temizler
    }

    void ekranCiz() {
        char ekran[YUKSEKLIK][GENISLIK];

        // Ekran� bo�luklarla doldur
        for (int i = 0; i < YUKSEKLIK; i++) {
            for (int j = 0; j < GENISLIK; j++) {
                ekran[i][j] = ' ';
            }
        }

        // Uzay gemisini yerle�tir
        ekran[oyuncu->getY()][oyuncu->getX() + 1] = '^';

        // Mermileri yerle�tir
        for (auto mermi : mermiler) {
            if (mermi->getY() >= 0)
                ekran[mermi->getY()][mermi->getX()] = '|';
        }

        // D��manlar� yerle�tir
        for (auto dusman : dusmanlar) {
            if (dusman->getY() < YUKSEKLIK)
                ekran[dusman->getY()][dusman->getX()] = 'X';
        }

        // Ekran� �er�eveyle �iz
        cout << " ";
        for (int i = 0; i < GENISLIK; i++) cout << "-";
        cout << endl;

        for (int i = 0; i < YUKSEKLIK; i++) {
            cout << "|"; // Sol s�n�r
            for (int j = 0; j < GENISLIK; j++) {
                cout << ekran[i][j];
            }
            cout << "|" << endl; // Sa� s�n�r
        }

        cout << " ";
        for (int i = 0; i < GENISLIK; i++) cout << "-";
        cout << endl;
    }

    void dusmanUret() {
        int x = rand() % (GENISLIK - 4) + 2; // Duvarlardan 1 birim uzakl�kta konumland�r
        dusmanlar.push_back(new Dusman(x, 0));
    }

    void carpismaKontrol() {
        for (size_t i = 0; i < mermiler.size(); i++) {
            for (size_t j = 0; j < dusmanlar.size(); j++) {
                if (mermiler[i]->getX() == dusmanlar[j]->getX() &&
                    mermiler[i]->getY() == dusmanlar[j]->getY()) {
                    // �arp��ma durumunda, mermi ve d��man� yok et
                    delete mermiler[i];
                    delete dusmanlar[j];
                    mermiler.erase(mermiler.begin() + i);
                    dusmanlar.erase(dusmanlar.begin() + j);
                    yokEdilenDusmanlar++; // Yok edilen d��man say�s�n� art�r
                    return;
                }
            }
        }
    }

    void geriSayim() {
        for (int i = 3; i > 0; i--) {
            ekranTemizle();
            cout << "Oyun " << i << " saniye icinde basliyor!" << endl;
            Sleep(1000);
        }
        ekranTemizle();
        cout << "Basla!" << endl;
        Sleep(1000);
        ekranTemizle();
    }

    void oyunBittiEkrani() {
        ekranTemizle();
        cout << "\n=================" << endl;
        cout << "   GAME OVER!   " << endl;
        cout << "=================" << endl;
        cout << "Yok edilen dusman sayisi: " << yokEdilenDusmanlar << endl; // Skor ekran�
    }

public:
    Oyun() {
        oyuncu = new UzayGemisi(GENISLIK / 2, YUKSEKLIK - 1); // Uzay gemisi ba�lang�� konumu
        calisiyor = true;
        srand(time(0)); // Rastgele d��man konumlar� i�in
    }

    ~Oyun() {
        delete oyuncu;
        for (auto mermi : mermiler) delete mermi;
        for (auto dusman : dusmanlar) delete dusman;
    }

    void calistir() {
        geriSayim();

        while (calisiyor) {
            // Klavye giri�i
            if (_kbhit()) {
                char tus = _getch();
                if (tus == 'a' || tus == 'd') oyuncu->hareketEt(tus);
                if (tus == ' ') mermiler.push_back(new Mermi(oyuncu->getX() + 1, oyuncu->getY() - 1));
                if (tus == 'q') calisiyor = false; // Oyunu bitir
            }

            // D��man �ret
            if (rand() % 15 == 0) dusmanUret();

            // D��manlar� daha yava� hareket ettir
            dusmanHizSayaci++;
            if (dusmanHizSayaci > 5) {
                for (auto dusman : dusmanlar) dusman->hareketEt();
                dusmanHizSayaci = 0;
            }

            // Mermileri g�ncelle
            for (auto mermi : mermiler) mermi->hareketEt();

            // �arp��ma kontrol�
            carpismaKontrol();

            // Ekran�n alt�na ula�an d��man kontrol�
            for (auto dusman : dusmanlar) {
                if (dusman->getY() >= YUKSEKLIK) {
                    calisiyor = false;
                    oyunBittiEkrani();
                }
            }

            // Ekran� temizle ve yeniden �iz
            ekranTemizle();
            ekranCiz();

            Sleep(100); // Oyun d�ng�s�n� yava�lat
        }

        // Oyun bitti�inde, sonu� ekran� g�ster
        oyunBittiEkrani();
    }
};

int main() {
    Oyun oyun;
    oyun.calistir();
    return 0;
}
